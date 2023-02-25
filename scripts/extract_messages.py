import re
import sys

inputFile = open(sys.argv[1], "r")
lines = inputFile.readlines()
inputFile.close()

resultMessages = []

for line in lines:
    match = re.search(
        r"^(?P<File>.*?\.(?P<Extension>cpp|h|c)):(?P<LineNumber>\d+):(?P<ColNumber>\d+): (?P<Type>warning|error): ("
        r"?P<Message>.*?) \[(?P<Note>.*?)]$", line)
    if match:
        file = match.group("File")
        line_number = match.group("LineNumber")
        col_number = match.group("ColNumber")
        message_type = match.group("Type")
        if message_type == "error":
            message_type = "failure"
        message = match.group("Message")
        note = match.group("Note")
        raw_details = line.replace('"', '\\"').replace("\n", "")
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
