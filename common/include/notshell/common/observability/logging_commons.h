#pragma once


#include <boost/log/trivial.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_impl.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include "project_config.h"

#define OTEL_TRACE_LENGTH 32
#define OTEL_SPAN_LENGTH 16

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace observability {

    class TraceIdAttrImpl :
        public logging::attribute::impl {
    private:
        std::string tracerName;

    public:
        TraceIdAttrImpl(std::string tracerName);

        logging::attribute_value get_value() override;
    };

    class TraceIdAttr :
        public logging::attribute {
    public:
        TraceIdAttr(std::string tracerName);
        explicit TraceIdAttr(attrs::cast_source const& source);
    };

    class SpanIdAttrImpl :
        public logging::attribute::impl {
    private:
        std::string tracerName;

    public:
        SpanIdAttrImpl(std::string tracerName);

        logging::attribute_value get_value() override;
    };

    class SpanIdAttr :
        public logging::attribute {
    public:
        SpanIdAttr(std::string tracerName);
        explicit SpanIdAttr(attrs::cast_source const& source);
    };

    class SpanFeeder : public sinks::basic_formatted_sink_backend<
        char,
        sinks::synchronized_feeding
    > {
    public:
        // The constructor initializes the internal data
        explicit SpanFeeder() {};

        // The function consumes the log records that come from the frontend
        void consume(logging::record_view const& rec, string_type const& command_line);
    };

    BOOST_LOG_GLOBAL_LOGGER(default_logger, src::severity_logger<logging::trivial::severity_level>)
    
    enum JsonFormat {
        NO_INDENT, FORMATTED
    };

    /**
     * Note: Array and nested object are not supported.
     */
    template< typename T >
    void to_json_node(std::ostream& strm, logging::attribute_value const& attr);

    void json_attrs_formatter(const logging::record_view &view, logging::formatting_ostream &os, JsonFormat mode);

    void initLogging();

}}

NOTSHELL_NAMESPACE_END
