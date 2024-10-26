#pragma once

#include <restbed>

#include "opentelemetry/trace/tracer.h"

#include "project_config.h"
#include "notshell/common/model/IJsonConvertible.h"

#include "notshell/common/http_helper.h"

namespace trace = opentelemetry::trace;

namespace model = dochkas::notshell::common::model;

NOTSHELL_NAMESPACE_BEGIN

namespace common {
    class RestController {
        protected:
            std::shared_ptr<trace::Tracer> tracer;
            std::shared_ptr<restbed::Service> service;

        public:
            RestController(std::shared_ptr<restbed::Service> service, std::shared_ptr<trace::Tracer> tracer)
                : service(service), tracer(tracer) {}

            /**
             * 1. Defines the resources
             * 2. Maps the methods to the resource handlers
             * 3. Maps the paths
             */
            virtual void init() = 0;

            template<class T,
                std::enable_if_t<std::is_base_of<model::IJsonConvertible<T>, T>::value, bool> = false>
            inline void entrypointHandler(const std::shared_ptr< restbed::Session > session,
                const std::function<std::shared_ptr<T>(const std::shared_ptr<const restbed::Request>)>& entityProviderHandler) {
                auto request = session->get_request();
                auto entity = entityProviderHandler(request);

                BUILD_JSON_RESPONSE(response, restbed::OK, entity)
                session->close(response);
            }

            template<class ConsumeType, class ProduceType>
            inline void entrypointEntityProducerHandler(const std::shared_ptr< restbed::Session > session,
                const std::function<std::unique_ptr<ConsumeType>(const std::vector<std::uint8_t>&)>& entityProducer,
                const std::function<std::shared_ptr<ProduceType>(const std::unique_ptr<ConsumeType>)>& entityProviderHandler) {
                auto request = session->get_request();
                size_t content_length = request->get_header( "Content-Length", 0 );

                session->fetch(content_length, [ request, &entityProducer, &entityProviderHandler ]
                ( const std::shared_ptr< restbed::Session > session, const restbed::Bytes & reqBody )
                {
                    auto reqEntity = entityProducer(reqBody);
                    auto respEntity = entityProviderHandler(std::move(reqEntity));

                    BUILD_JSON_RESPONSE(response, restbed::OK, respEntity)
                    session->close(response);
                });
            }

            std::shared_ptr<restbed::Service> getService() const {
                return this->service;
            }
    };
}

NOTSHELL_NAMESPACE_END