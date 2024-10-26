#pragma once

#define BUILD_JSON_RESPONSE(var_name, status_code, entity) \
    restbed::Response var_name; \
    { \
        auto response_body = entity->serialize_json(); \
        var_name.set_body(response_body); \
        var_name.add_header("Content-Length", std::to_string(response_body.size())); \
        var_name.add_header("Content-Type", "application/json"); \
        var_name.set_status_code(status_code); \
    }

#define JSON_POPULATE_COLLECTION_NODE(nlohmann_node_j, item_collection, ItemType) \
    auto new_item = std::make_shared<ItemType>(); \
    new_item->from_json(nlohmann_node_j, *new_item); \
    item_collection.push_back(new_item); \

#define JSON_DEFINE_ARRAY_METHODS(Type, collection_field, ItemType) \
    void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) \
    { \
        nlohmann_json_j = json::array(); \
        for (auto& nlohmann_convertible : nlohmann_json_t.collection_field) { \
            json convertibl_node; \
            nlohmann_convertible->to_json(convertibl_node, *nlohmann_convertible); \
            nlohmann_json_j.push_back(convertibl_node); \
        } \
    } \
    void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) \
    { \
        nlohmann_json_t.collection_field.clear(); \
        if (nlohmann_json_j.is_array()) { \
            for (const auto& convertible_node : nlohmann_json_j) { \
                JSON_POPULATE_COLLECTION_NODE(convertible_node, nlohmann_json_t.collection_field, ItemType) \
            } \
        } else if (nlohmann_json_j.is_object()) { \
            JSON_POPULATE_COLLECTION_NODE(nlohmann_json_j, nlohmann_json_t.collection_field, ItemType) \
        } else { \
            throw std::runtime_error("Bad json format for type Type"); \
        } \
    }

#define BUILD_GENERIC_HANDLER(handler_var_name, handle_method_ref, entity_type_name) \
    std::function<void(const std::shared_ptr< restbed::Session > session)> handler_var_name; \
    { \
        auto method_entity_provider = std::bind(handle_method_ref, *this, std::placeholders::_1); \
        handler_var_name = [this, method_entity_provider] (const std::shared_ptr< restbed::Session > session) { \
            this->entrypointHandler<entity_type_name>(session, method_entity_provider); \
        }; \
    }

#define BUILD_ENTITY_CONSUMER_HANDLER(handler_var_name, handle_method_ref, entity_type_name) \
    std::function<void(const std::shared_ptr< restbed::Session > session)> handler_var_name; \
    { \
        auto method_entity_producer = [] (const std::vector<std::uint8_t>& body) { \
            auto entity = std::make_unique<entity_type_name>(); \
            entity->deserialize_json(std::string(body.begin(), body.end())); \
            return entity; \
        }; \
        auto method_entity_provider = std::bind(handle_method_ref, *this, std::placeholders::_1); \
        handler_var_name = [this, method_entity_producer, method_entity_provider] (const std::shared_ptr< restbed::Session > session) { \
            this->entrypointEntityProducerHandler<entity_type_name, entity_type_name>(session, method_entity_producer, method_entity_provider); \
        }; \
    }
