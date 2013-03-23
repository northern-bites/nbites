from pyparsing import *

field_flavors = [
    Keyword('required'),
    Keyword('optional'),
    Keyword('repeated')
]

primitive_field_types = [
    Keyword('string'),
    Keyword('bytes'),
    Keyword('int32'),
    Keyword('int64'),
    Keyword('uint32'),
    Keyword('uint64'),
    Keyword('sint32'),
    Keyword('sint64'),
    Keyword('fixed32'),
    Keyword('fixed64'),
    Keyword('sfixed32'),
    Keyword('sfixed64'),
    Keyword('float'),
    Keyword('double'),
    Keyword('bool'),
    Keyword('message'),
    Keyword('enum')]

# allow underscores in names
name_word = Word(alphanums + '_')

default = Literal('[') + Literal('default') + Literal('=') + name_word + ']'

field_types = primitive_field_types + [name_word]

field_flavor = MatchFirst(field_flavors).setResultsName('flavor')
field_type = MatchFirst(field_types).setResultsName('type')
field_name = name_word.setResultsName('name')
field_id = Word( nums ).setResultsName('id')
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

messages = ZeroOrMore(Group(message))

messages.ignore(dblSlashComment)

def parse_proto_file(file_path):
    return messages.parseFile(file_path)
