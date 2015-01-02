/*
Mark Down to HTML converter
Bryan Worrell
3/9/14
---------------------------

This function implements basic logic for converting some markdown text into html.
There are some edge cases that, given more time, can be added.
 
Program Assumptions
-------------------
An int main() loads an input and calls markdown_to_html with the it.
 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdint.h>

const char *markdown_to_html(const char *input);

// An enumeration for all possible states the parser and its tags can be in.
enum state {TAG_OPEN, TAG_CLOSE};

struct ParserStatus
{
    state paragraphTag;
    state italicTag;
    state boldTag;
    state listTag;
    state ulistTag;
    state olistTag;
    
    ParserStatus()
    {
        paragraphTag = italicTag = boldTag = listTag = ulistTag = olistTag = TAG_CLOSE;
    }
};

const char *markdown_to_html(const char *input)
{
    int hashCount = 0;
    std::string output;
    ParserStatus parser;
    
    for (uint i=0; input[i] != '\0'; ++i)
    {
        switch (input[i])
        {
            case '#':
                // Count how many hash tags to determine header level
                while (input[i] == '#')
                {
                    ++hashCount;
                    ++i;
                }
                
                // Open header
                output.append("<h" + std::to_string(hashCount) + ">");
                
                // Write out rest of header and ignore any other hash tags
                while (input[i] != '\n')
                {
                    if (input[i] != '#' && input[i] != ' ')
                        output.push_back(input[i]);
                    
                    ++i;
                }
                
                // Close header
                output.append("</h" + std::to_string(hashCount) + ">");
                output.push_back(input[i]);
                hashCount = 0;
                
                break;
                
            case '+':
            case '-':
                
                ++i;
                
                // If next char is space or tab, it's a list
                if ((input[i] == ' ' || input[i] == '\t') && parser.paragraphTag != TAG_OPEN)
                {
                    // Check if beginning new list
                    if (parser.ulistTag == TAG_CLOSE)
                    {
                        parser.ulistTag = TAG_OPEN;
                        parser.listTag = TAG_OPEN;
                        output.append("<ul>");
                        output.push_back('\n');
                        output.append("<li>");
                    }
                    else
                    {
                        parser.listTag = TAG_OPEN;
                        output.append("<li>");
                    }
                }
                // Otherwise not a list, just print the char
                else
                    output.push_back(input[i]);
                
                break;
            
            case '*':
            case '_':
                
                // Check next char to determine correct action
                ++i;
                
                // If next char is space or tab, it's a list
                if ((input[i] == ' ' || input[i] == '\t') && parser.paragraphTag != TAG_OPEN)
                {
                    // Check if beginning new list
                    if (parser.ulistTag == TAG_CLOSE)
                    {
                        parser.ulistTag = TAG_OPEN;
                        parser.listTag = TAG_OPEN;
                        output.append("<ul>");
                        output.push_back('\n');
                        output.append("<li>");
                    }
                    else
                    {
                        parser.listTag = TAG_OPEN;
                        output.append("<li>");
                    }
                }
                // If next char is * or _ then it's a bold tag
                else if (input[i] == '*' || input[i] == '_')
                {
                    // Make sure it's not a new paragraph
                    if (parser.paragraphTag == TAG_CLOSE)
                    {
                        output.append("<p>");
                        parser.paragraphTag = TAG_OPEN;
                    }
                    
                    if (parser.boldTag == TAG_CLOSE)
                    {
                        output.append("<strong>");
                        parser.boldTag = TAG_OPEN;
                    }
                    else
                    {
                        output.append("</strong>");
                        parser.boldTag = TAG_CLOSE;
                    }
                }
                // Otherwise next char is alphabetical and it's an italic tag
                else
                {
                    // Make sure it's not a new paragraph
                    if (parser.paragraphTag == TAG_CLOSE)
                    {
                        output.append("<p>");
                        parser.paragraphTag = TAG_OPEN;
                    }
                    
                    if (parser.italicTag == TAG_CLOSE)
                    {
                        output.append("<em>");
                        parser.italicTag = TAG_OPEN;
                        --i;
                    }
                    else
                    {
                        output.append("</em>");
                        parser.italicTag = TAG_CLOSE;
                        --i;
                    }
                }
                
                break;
                
            case '&':
                if (input[i-1] != ' ')
                    output.append("&amp;");
                else
                    output.push_back('&');
                
                break;
                
            case '<':
                output.append("&lt;");
                
                break;
                
            case '\n':
                
                // Check to see if currently building an unordered list
                if (parser.ulistTag == TAG_OPEN)
                {
                    // Check to see if the list is ending
                    if (input[i+1] == '\n')
                    {
                        output.append("</li>");
                        output.push_back('\n');
                        output.append("</ul>");
                        output.push_back('\n');
                        parser.ulistTag = TAG_CLOSE;
                        parser.listTag = TAG_CLOSE;
                    }
                    // End current line
                    else
                    {
                        output.append("</li>");
                        output.push_back('\n');
                    }
                }
                // Check to see if currently building an ordered list
                else if (parser.olistTag == TAG_OPEN)
                {
                    // Check to see if the list is ending
                    if (input[i+1] == '\n')
                    {
                        output.append("</li>");
                        output.push_back('\n');
                        output.append("</ol>");
                        output.push_back('\n');
                        parser.olistTag = TAG_CLOSE;
                        parser.listTag = TAG_CLOSE;
                    }
                    // End current line
                    else
                    {
                        output.append("</li>");
                        output.push_back('\n');
                    }
                }
                // Check to see if ending a paragraph
                else if (parser.paragraphTag == TAG_OPEN && input[i+1] == '\n')
                {
                    output.append("</p>");
                    output.push_back('\n');
                    parser.paragraphTag = TAG_CLOSE;
                }
                else if (parser.paragraphTag == TAG_CLOSE && isalpha(input[i+1]))
                {
                    output.push_back(input[i]);
                    output.append("<p>");
                    parser.paragraphTag = TAG_OPEN;
                }
                else
                    output.push_back(input[i]);
                
                break;
                
            default:
                
                // Check if the character is an integer beginning an ordered list
                if (isdigit(input[i]) && input[i+1] == '.' &&
                    (input[i+2] == ' ' || input[i+2] == '\t') &&
                    parser.paragraphTag != TAG_OPEN)
                {
                    if (parser.olistTag == TAG_CLOSE)
                    {
                        parser.olistTag = TAG_OPEN;
                        parser.listTag = TAG_OPEN;
                        output.append("<ol>");
                        output.push_back('\n');
                        output.append("<li>");
                    }
                    else
                    {
                        parser.listTag = TAG_OPEN;
                        output.append("<li>");
                    }

                    i += 2;
                }
                // Not an integer, so just print the char
                else
                    output.push_back(input[i]);
        }
	}
    
    // Add closing paragraph tag if needed at end of output
    if (parser.paragraphTag == TAG_OPEN)
    {
        output.pop_back();
        output.append("</p>");
        output.push_back('\0');
    }
    else
        output.push_back('\0');
    
    return output.c_str(); // return output as const char*
}
