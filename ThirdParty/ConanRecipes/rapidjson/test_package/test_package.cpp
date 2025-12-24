#include <rapidjson/document.h>

int main(void) {
    const char* json = "{\"a\": true, \"b\": 1, \"c\": \"2\"}";
    rapidjson::Document document;
    document.Parse(json);
    assert(document.IsObject());
    return 0;
}
