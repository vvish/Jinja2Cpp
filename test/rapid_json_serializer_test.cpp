#include "gtest/gtest.h"

#include "../src/rapid_json_serializer.h"

namespace
{
template<typename T>
jinja2::InternalValue MakeInternalValue(T&& v)
{
    return jinja2::InternalValue(std::forward<T>(v));
}
}
TEST(RapidJsonSerializerTest, SerializeTrivialTypes)
{
    const jinja2::rapidjson_serializer::DocumentWrapper document;

    const auto stringValue = document.CreateValue(MakeInternalValue<std::string>("string"));
    const auto intValue = document.CreateValue(MakeInternalValue<int64_t>(123));
    const auto doubleValue = document.CreateValue(MakeInternalValue(12.34));

    EXPECT_EQ("\"string\"", stringValue.AsString());
    EXPECT_EQ("123", intValue.AsString());
    EXPECT_EQ("12.34", doubleValue.AsString());
}

TEST(RapidJsonSerializerTest, SerializeComplexTypes)
{
    jinja2::rapidjson_serializer::DocumentWrapper document;
    {
        jinja2::InternalValueMap params = {{"string", MakeInternalValue<std::string>("hello")}};
        const auto jsonValue = document.CreateValue(CreateMapAdapter(std::move(params)));
        EXPECT_EQ("{\"string\":\"hello\"}", jsonValue.AsString());
    }
    {
        jinja2::InternalValueMap params = {{"int", MakeInternalValue<int64_t>(123)}};
        const auto jsonValue = document.CreateValue(CreateMapAdapter(std::move(params)));
        EXPECT_EQ("{\"int\":123}", jsonValue.AsString());
    }

    {
        jinja2::InternalValueList array {MakeInternalValue<int64_t>(1), MakeInternalValue<int64_t>(2), MakeInternalValue<int64_t>(3)};
        jinja2::InternalValueMap map {{"string", MakeInternalValue<std::string>("hello")}};
        jinja2::InternalValueMap params = {{"array", jinja2::ListAdapter::CreateAdapter(std::move(array))}, {"map", CreateMapAdapter(std::move(map))}};
        const auto jsonValue = document.CreateValue(CreateMapAdapter(std::move(params)));
        EXPECT_EQ("{\"map\":{\"string\":\"hello\"},\"array\":[1,2,3]}", jsonValue.AsString());
    }
}

TEST(RapidJsonSerializerTest, SerializeComplexTypesWithIndention)
{
    const jinja2::rapidjson_serializer::DocumentWrapper document;

    jinja2::InternalValueList array {MakeInternalValue<int64_t>(1), MakeInternalValue<int64_t>(2), MakeInternalValue<int64_t>(3)};
    jinja2::InternalValueMap map {{"string", MakeInternalValue<std::string>("hello")}};
    jinja2::InternalValueMap params = {
        {"array", jinja2::ListAdapter::CreateAdapter(std::move(array))},
        {"map", CreateMapAdapter(std::move(map))}
    };
    const auto jsonValue = document.CreateValue(CreateMapAdapter(std::move(params)));

    auto indentedDocument =
R"({
    "map": {
        "string": "hello"
    },
    "array": [1, 2, 3]
})";

    EXPECT_EQ(indentedDocument, jsonValue.AsString(4));
}
