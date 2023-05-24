import sys

def add_comment_header(file_name, desc):
    comment_header = f'''/*
   File: {file_name}
   Description: {desc}
   Author: Richard Váňa
   Date: May 23, 2023
*/'''

    try:
        with open(file_name, 'r+') as file:
            content = file.read()
            file.seek(0, 0)
            file.write(comment_header.rstrip('\r\n') + '\n\n' + content)
    except FileNotFoundError:
        print(f"File '{file_name}' not found.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python cheader.py <file_name> <desc>")
    else:
        file_name = sys.argv[1]
        desc = sys.argv[2]
        add_comment_header(file_name, desc)
