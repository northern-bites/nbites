#!/usr/bin/python
import sys, getopt, os.path

import parser
import boostifier
import templates

def basename_no_extension(file_path):
    return os.path.basename(file_path).rsplit('.')[:-1][0]

def boost_output_filename(name):
    return '.'.join([name] + ['boost', 'pb', 'cpp'])

def protoc_header_filename(name):
    return '.'.join([name] + ['pb', 'h'])

def extract_message_names(parsed_proto_message):
    message_names = [parsed_proto_message.name]
    # right now toss in messages and nested messages together
    for element in parsed_proto_message.elements:
        if 'message' in element:
            message_names += extract_message_names(element)

    return message_names

def extract_enum_names(parsed_proto_message):
    enum_names = []
    # right now toss in messages and nested messages together
    for element in parsed_proto_message.elements:
        if 'enum' in element:
            enum_names += [element.name]
        if 'message' in element:
            enum_names += extract_enum_names(element)

    return enum_names

def main(argv):

    output_dir = ''
    input_file_path = ''

    USAGE = 'Usage: test.py [-o <output_dir>] <inputfile>'

    try:
        opts, args = getopt.getopt(argv,"ho:",["output="])
    except getopt.GetoptError:
        print USAGE
        sys.exit(2)

    if len(args) < 1:
        print 'Need to specify at least an input file!'
        print USAGE
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print USAGE
            sys.exit()
        elif opt in ("-o", "--output"):
            output_dir = arg

    for input_file_path in args:

        input_basename = basename_no_extension(input_file_path)

        output_file = os.path.join(output_dir, boost_output_filename(input_basename))

        parsed_proto_file = parser.parse_proto_file(input_file_path)

        if len(parsed_proto_file.messages) == 0:
            print 'Warning! Could not parse any messages out of %(file)s! If that\'s ' \
                'not expected, that means there might be a bug with the parser!' \
                % { 'file': input_file_path }

        known_messages = []
        known_enums = []
        wrapped_messages = []
        scopes = []

        if parsed_proto_file.package != '':
            scopes = list(parsed_proto_file.package.scopes)

        for message in parsed_proto_file.messages:
            known_messages += extract_message_names(message)
            known_enums += extract_enum_names(message)
            wrapped_messages += boostifier.process_message(message, known_messages, known_enums, scopes)

        output_file = open(output_file, 'w')

        proto_header_file = protoc_header_filename(input_basename)
        output_file.write(templates.HEADER % locals())

        for scope in scopes:
            scope_name = scope
            output_file.write(templates.PACKAGE_SCOPE_DUMMY_CLASS % locals())

        module_name = input_basename + '_proto'
        output_file.write(templates.MODULE_DECLARATION % locals())

        for scope in scopes:
            scope_name = scope
            output_file.write(templates.PACKAGE_SCOPE % locals())

        for wrapped_message in wrapped_messages:
            output_file.write(wrapped_message)

        output_file.write(templates.FOOTER)

if __name__ == "__main__":
    main(sys.argv[1:])
