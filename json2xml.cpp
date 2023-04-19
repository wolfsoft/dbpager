#include <libxml/parser.h>
#include <libxml/tree.h>
#include <json/json.h>

#include <string>
#include <iostream>

using namespace Json;

void write_value(xmlNodePtr n, Json::Value &value) {
	switch (value.type()) {
		case nullValue:
			break;
		case booleanValue:
		case intValue:
		case uintValue:
		case realValue:
		case stringValue: {
			std::string v = value.asString();
			xmlNodeSetContent(n, (const xmlChar*)(v.c_str()));
			break;
		}
		case arrayValue: {
			ArrayIndex size = value.size();
			for (ArrayIndex index = 0; index < size; ++index) {
				xmlNodePtr c = xmlNewChild(n, NULL, BAD_CAST "element", NULL);
				write_value(c, value[index]);
			}
			break;
		}
		case objectValue: {
			Value::Members members(value.getMemberNames());
			for (auto it = members.begin(); it != members.end(); ++it) {
				xmlNodePtr c = xmlNewChild(n, NULL, (const xmlChar*)it->c_str(), NULL);
				write_value(c, value[*it]);
			}
			break;
		}
	}
}

xmlDocPtr parse_json(Json::Value &src) {
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr n = xmlNewNode(NULL, BAD_CAST "root");
	write_value(n, src);
	xmlDocSetRootElement(doc, n);
	return doc;
}

int main() {
	Json::Value root;
	Json::Reader reader;
	reader.parse(std::cin, root);

	xmlDocPtr doc = parse_json(root);

	xmlChar *xmlbuff;
	int buffersize;
	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
	printf("%s", (char *) xmlbuff);
	xmlFree(xmlbuff);
}
