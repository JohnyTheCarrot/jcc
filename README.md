# A WIP C frontend using LLVM as the backend

![Screenshot_20250225_183929](https://github.com/user-attachments/assets/0b271888-c620-4b5a-b4e4-8a368d48f92c)

## Noteworthy Technical Details
- My compiler works in a lazy manner. The parser requests a token from the preprocessor, which requests a token from the tokenizer, which asks for characters from the character stream.
  Yes, you read that right. My preprocessor and tokenizer stages are swapped. The reason for this is so I can retain accurate spans, as well as allowing the process to be lazy and O(n) at all.
  The behavior is still as if it were first preprocessed and then tokenized.
- Iterators! I love iterators! My character stream is an input iterator, but so is my tokenizer and preprocessor.
  This allows me to write a for each loop over my preprocessor which will, as it increments the iterator, lazily tokenize and then yield the next token. It also allows for easy integration with the STL algorithms.
  
  Look at how cool this is:
  ```cpp
  for (auto const &token : preprocessor) { std::cout << token << ' '; }
  ```

### Details on the Tokenizer & Preprocessor
This whole part of my compiler is of linear complexity where n is equal to the number of input characters.
Tokenization of keywords and punctuators are done using an (admittedly poor) implementation of the prefix trie data structure.
This is a trade off between raw speed and ease of maintainability. I imagine the fastest tokenization here would be a big ol' switch statement.
That would be a maintenance nightmare however. A prefix trie is a worthwhile trade off.
The idea is that each character in the input translation unit is only seen once. We don't backtrack.
We are "in the moment" in that once you consume a character, there's no going back.
Context for what came before is "encoded" in your current code path.
This same principle applies to preprocessing. Each and every token is only seen once, and then forgotten about if not used by the callee of the preprocessor.

Swapping the preprocessor and tokenizer is done with the purpose of not doing a second pass over a given input file, allowing streaming.
Theoretically, one could do these two stages **live, while they are typing it into a console**.
This is achieved by, in addition to normal tokens, also tokenizing the preprocessor tokens.
This allows the preprocessor to intercept these tokens, execute them and repeat until it yields a valid normal token.
Macros and include files are stack-based. Including a file pushes a new tokenizer to the stack of tokenizers, and this tokenizer will be popped off when exhausted.
This makes knowing where each and every token came from trivial.
Macros operate in practically the exact same way.

## Known Issues / TODOs
I won't be updating this much because these should be issues but:

- the prefix trie implementation is incredibly memory inefficient
- preprocessor directives don't currently allow spaces between the # and the directive
- the preprocessor isn't fully std compliant yet. working on that.
- PascalCase file names are an unofficial give-away that it is an old bit of code that needs refactoring.
