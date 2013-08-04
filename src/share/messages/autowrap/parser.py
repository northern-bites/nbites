from pyparsing import *
import protobuf_defs

# allow underscores in names
name_word = Word(alphanums + '_')

package_scopes = delimitedList(name_word, '.')
package_declaration = 'package' + package_scopes.setResultsName('scopes') + ';'

default_value_word = Word(alphanums + '-_.\"\\ ')
default = Literal('[') + Literal('default') + Literal('=') + default_value_word + ']'

field_flavors = [
    Keyword('required'),
    Keyword('optional'),
    Keyword('repeated')
]

field_types = []

for type in protobuf_defs.ALL_TYPES:
    field_types += [Keyword(type)]

# allow enum and message types
field_types += [name_word]

field_flavor = MatchFirst(field_flavors).setResultsName('flavor')
field_type = MatchFirst(field_types).setResultsName('type')
field_name = name_word.setResultsName('name')
field_id = Word(nums).setResultsName('id')
field = field_flavor + field_type + field_name + Literal('=') + \
    field_id + Optional(default) + ';'

enum_item_name = name_word.setResultsName('name')
enum_item_value = Word(nums).setResultsName('value')
enum_item = enum_item_name + Literal('=') + enum_item_value + ';'

enum_keyword = Keyword('enum').setResultsName('enum')
enum_name = name_word.setResultsName('name')
enum_items = ZeroOrMore(Group(enum_item)).setResultsName('elements')

enum = enum_keyword + enum_name + '{' + enum_items + '}'

message = Forward()

nested_element = field | enum | message
nested_elements = ZeroOrMore(Group(nested_element)).setResultsName('elements')

message_keyword = Keyword('message').setResultsName('message')
message_name = name_word.setResultsName('name')
message << message_keyword + message_name + '{' + nested_elements + '}'

# messages in a proto_file are named elements so that top-level messages can
# be processed just like nested messages (so they're nested under the global
# namespace in some sense)
proto_file = Optional(package_declaration).setResultsName('package') + ZeroOrMore(Group(message)).setResultsName('elements')

proto_file.ignore(dblSlashComment)

def parse_proto_file(file_path):
    return proto_file.parseFile(file_path)
