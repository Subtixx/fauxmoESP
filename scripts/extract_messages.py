import re
import sys

inputFile = open(sys.argv[1], "r")
fileContents = inputFile.read()
inputFile.close()

resultMessages = []

for match in re.finditer(
        r"^In file included (from .*:[0-9]+[,:])(?s:.)+?\n(?P<File>.*?\.(?P<Extension>cpp|h|c)):(?P<LineNumber>\d+):(?P<ColNumber>\d+): (?P<Type>warning|error): (?P<Message>.*?)\[(?P<Note>.*?)]\n(?s:.+?)\n(?s:.+?)$",
        fileContents, re.MULTILINE):
    if match:
        file = match.group("File")
        line_number = match.group("LineNumber")
        col_number = match.group("ColNumber")
        message_type = match.group("Type")
        if message_type == "error":
            message_type = "failure"
        message = match.group("Message")
        note = match.group("Note")
        # Get line from match
        raw_details = match.group(0).replace('"', '\\"').replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t").replace("    ", "\\t")
        resultMessages.append(
            '{"file": "%s", "line": %s, "start_column": %s, "end_column": %s, "annotation_level": "%s", '
            '"title": "%s", "message": "%s", "raw_details": "%s"}' % (
                file, line_number, col_number, col_number, message_type, note, message, raw_details))
#    else:
#        print("No match found for line: %s" % line)

output = open(sys.argv[2], "w")
output.write("[")
output.write(", ".join(resultMessages))
output.write("]")
output.close()
