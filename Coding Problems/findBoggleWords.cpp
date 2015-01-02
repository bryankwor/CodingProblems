#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <stack>
#include <vector>
#include <unordered_set>

const std::string dictionaryFileName = "english.txt";

// Struct representing characteristics of a letter in the game board (trie node)
struct letterNode
{
    uint16_t xPos;
    uint16_t yPos;
    char letter;
    std::vector<letterNode> child; // Array of adjacent letters
};

const char *findWords(uint16_t, uint16_t, char **); // Find all possible words
void loadDictionary(std::unordered_set<std::string> &); // Load dictionary into hash table
void pushNeighbors(const uint16_t, const uint16_t, letterNode &, char **, bool **);

const char *findWords(const uint16_t &boardWidth, const uint16_t &boardHeight, char **letters)
{
	bool **active; // Matrix of bools to indicate what letters have been visited
	bool pushed = false;
	letterNode *root = new letterNode; // A "trie" that will be traversed
	letterNode newNode;
	std::string word; // Word to be checked in dictionary
	std::string words; // Words found on board
	std::unordered_set<std::string> dictionary;
	std::vector<letterNode>::iterator current; // Trie traversal pointer
	std::stack<std::vector<letterNode>::iterator> visited; // Stack used for "Depth First Searching"
	std::vector<letterNode>::iterator index;
	std::vector<letterNode>::iterator i;

	loadDictionary(dictionary);

	// Initialize boolean matrix and children of Trie root
	active = new bool*[boardWidth];
	for (uint16_t i = 0; i < boardHeight; ++i)
	{
		active[i] = new bool;
	}
	for (uint16_t i = 0; i < boardWidth; ++i)
	{
		for (uint16_t j = 0; j < boardHeight; ++j)
		{
			newNode.letter = letters[i][j];
			newNode.xPos = i;
			newNode.yPos = j;
			root->child.push_back(newNode);

			active[i][j] = false;
		}
	}
    // Trie root's children is all the letters in the game board.
    // Start at each of these letters and perform a "Depth First Search" type of
    // algorithm to find all possible paths they can take.
    for (index=root->child.begin(); index != root->child.end(); ++index)
    {
        // Push letter to start as a source
        visited.push(index);
            
        // Search all possible paths that this letter can take
        do
        {
            // Get element at top of stack and mark it active
            current = visited.top();
            active[current->xPos][current->yPos] = true;
                
            // If current element has no children, push all of it's neighbors that aren't already active
            if (current->child.empty())
            {
				pushNeighbors(boardWidth, boardHeight, *current, letters, active);
				word.push_back(current->letter); // Append current letter to word being formed
                    
                // Push the children (aka neighbors) of the current letter onto the stack
                for (i = current->child.begin(); i != current->child.end(); ++i)
                {
                    visited.push(i);
                    pushed = true;
                }
            }
                
            // If no nodes were pushed on the stack, we hit the deepest node for the current path
            if (!pushed)
            {
                // Deactivate this letter so it can be used in a different path
                active[current->xPos][current->yPos] = false;
                    
                // Pop current letter and back up to its parent
                visited.pop();
                    
                // Since we're at the deepest node, see if it's an actual word in dictionary, takes O(1) time
                if (dictionary.count(word) && word.size() > 2)
                {
                    words.append(word);
                    words.push_back('\n');
                }
                    
                // Remove letter as we're moving to a different path
                word.pop_back();
            }
            else
                pushed = false;
                
        }
        while (!visited.empty());
    }
        
    words.push_back('\0');
    return words.c_str();
}

void loadDictionary(std::unordered_set<std::string> &dictionary)
{
    std::string word;
    
    // Open dictionary file
    std::ifstream fin(dictionaryFileName.c_str());
    
    // Store all elements of dictionary in the hash table
	if (fin.good())
	{
		while (fin >> word)
		{
			dictionary.insert(word);
		}
    }
	else
	{
		// TODO: Assert(missing dictionary file!)
	}
}

void pushNeighbors(const uint16_t &boardWidth, const uint16_t &boardHeight, letterNode &node, char **letters, bool **active)
{
    letterNode newNode;
    
    // Push all neighbors adjacent to letter's x and y positions IF they haven't been visited yet
    for (int16_t i=-1; i<=1; ++i)
    {
        for (int16_t j=-1; j<=1; ++j)
        {
            // Check to prevent array boundary edge cases
            if (node.xPos+i >= 0 && node.xPos+i < boardWidth &&
                node.yPos+j >= 0 && node.yPos+j < boardHeight)
            {
                // Check if already visited
                if (!active[node.xPos+i][node.yPos+j])
                {
                    newNode.letter = letters[node.xPos+i][node.yPos+j];
                    newNode.xPos = node.xPos + i;
                    newNode.yPos = node.yPos + j;
                    node.child.push_back(newNode);
                }
            }
        }
    }
}