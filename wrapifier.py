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

    output_file = ''
    input_file_path = ''

    try:
        opts, args = getopt.getopt(argv,"ho:",["output="])
    except getopt.GetoptError:
        print 'Usage: test.py [-o <outputfile>] <inputfile>'
        sys.exit(2)

    if len(args) != 1:
        print 'Need to specify an input file!'
        print 'Usage: test.py [-o <outputfile>] <inputfile>'
        sys.exit(2)
    input_file_path = args[0]

    for opt, arg in opts:
        if opt == '-h':
            print 'Usage: test.py [-o <outputfile>] <inputfile>'
            sys.exit()
        elif opt in ("-o", "--output"):
            output_file = arg

    input_basename = basename_no_extension(input_file_path)

    if output_file == '':
        output_file = boost_output_filename(input_basename)

    parsed_proto_messages = parser.parse_proto_file(input_file_path)

    known_messages = []
    known_enums = []
    wrapped_messages = []

    for message in parsed_proto_messages:
        known_messages += extract_message_names(message)
        known_enums += extract_enum_names(message)
        wrapped_messages += boostifier.process_message(message, known_messages, known_enums)

    output_file = open(output_file, 'w')

    proto_header_file = protoc_header_filename(input_basename)
    module_name = input_basename + '_proto'
    output_file.write(templates.HEADER % locals())

    for wrapped_message in wrapped_messages:
        output_file.write(wrapped_message)

    output_file.write(templates.FOOTER)

if __name__ == "__main__":
    main(sys.argv[1:])
