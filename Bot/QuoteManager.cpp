#include "QuoteManager.hpp"
#include <cstdlib>

QuoteManager::QuoteManager() : _quotes()
{
}

QuoteManager::~QuoteManager()
{
}

void QuoteManager::loadQuotes()
{
  Quote q1;
  q1.category = "cpp";
  q1.text = "C makes it easy to shoot yourself in the foot; C++ makes it harder, but when you do, it blows away your whole leg.";
  q1.author = "Bjarne Stroustrup";

  Quote q2;
  q2.category = "general";
  q2.text = "Talk is cheap. Show me the code.";
  q2.author = "Linus Torvalds";

  Quote q3;
  q3.category = "general";
  q3.text = "Programs must be written for people to read, and only incidentally for machines to execute.";
  q3.author = "Harold Abelson";

  Quote q4;
  q4.category = "cpp";
  q4.text = "The C++ Standard Library is a library of containers, iterators, algorithms and function objects.";
  q4.author = "Bjarne Stroustrup";

  Quote q5;
  q5.category = "c";
  q5.text = "C is quirky, flawed, and an enormous success.";
  q5.author = "Dennis Ritchie";

  Quote q6;
  q6.category = "c";
  q6.text = "The spirit of C is simplicity.";
  q6.author = "Dennis Ritchie";

  Quote q7;
  q7.category = "c";
  q7.text = "C is not a big language, and it is not well served by a big book.";
  q7.author = "Dennis Ritchie";

  Quote q8;
  q8.category = "cpp";
  q8.text = "Within C++, there is a much smaller and cleaner language struggling to get out.";
  q8.author = "Bjarne Stroustrup";

  Quote q9;
  q9.category = "cpp";
  q9.text = "C++ is designed to allow you to express ideas directly and efficiently.";
  q9.author = "Bjarne Stroustrup";

  Quote q10;
  q10.category = "general";
  q10.text = "Bad programmers worry about the code. Good programmers worry about data structures and their relationships.";
  q10.author = "Linus Torvalds";

  Quote q11;
  q11.category = "general";
  q11.text = "I'm doing a (public) 'NVIDIA, fuck you!'";
  q11.author = "Linus Torvalds";

  Quote q12;
  q12.category = "general";
  q12.text = "Any fool can write code that a computer can understand. Good programmers write code that humans can understand.";
  q12.author = "Martin Fowler";

  Quote q13;
  q13.category = "general";
  q13.text = "Code is like humor. When you have to explain it, it's bad.";
  q13.author = "Cory House";

  Quote q14;
  q14.category = "general";
  q14.text = "Before software can be reusable it first has to be usable.";
  q14.author = "Ralph Johnson";

  Quote q15;
  q15.category = "general";
  q15.text = "Fix the cause, not the symptom.";
  q15.author = "Steve Maguire";

  Quote q16;
  q16.category = "general";
  q16.text = "Simplicity is the soul of efficiency.";
  q16.author = "Austin Freeman";

  Quote q17;
  q17.category = "general";
  q17.text = "Walking on water and developing software from a specification are easy if both are frozen.";
  q17.author = "Edward V. Berard";

  Quote q18;
  q18.category = "general";
  q18.text = "The best error message is the one that never shows up.";
  q18.author = "Thomas Fuchs";

  Quote q19;
  q19.category = "general";
  q19.text = "Measuring programming progress by lines of code is like measuring aircraft building progress by weight.";
  q19.author = "Bill Gates";

  Quote q20;
  q20.category = "general";
  q20.text = "Controlling complexity is the essence of computer programming.";
  q20.author = "Brian Kernighan";

  Quote q21;
  q21.category = "general";
  q21.text = "Debugging is twice as hard as writing the code in the first place.";
  q21.author = "Brian Kernighan";

  Quote q22;
  q22.category = "general";
  q22.text = "The most disastrous thing that you can ever learn is your first programming language.";
  q22.author = "Alan Kay";

  Quote q23;
  q23.category = "general";
  q23.text = "Premature optimization is the root of all evil.";
  q23.author = "Donald Knuth";

  Quote q24;
  q24.category = "cpp";
  q24.text = "There are only two kinds of languages: the ones people complain about and the ones nobody uses.";
  q24.author = "Bjarne Stroustrup";

  Quote q25;
  q25.category = "general";
  q25.text = "UNIX is basically a simple operating system, but you have to be a genius to understand the simplicity.";
  q25.author = "Dennis Ritchie";

  Quote q26;
  q26.category = "general";
  q26.text = "Perfection is achieved, not when there is nothing more to add, but when there is nothing left to take away.";
  q26.author = "Antoine de Saint-Exupéry";

  _quotes.push_back(q1);
  _quotes.push_back(q2);
  _quotes.push_back(q3);
  _quotes.push_back(q4);
  _quotes.push_back(q5);
  _quotes.push_back(q6);
  _quotes.push_back(q7);
  _quotes.push_back(q8);
  _quotes.push_back(q9);
  _quotes.push_back(q10);
  _quotes.push_back(q11);
  _quotes.push_back(q12);
  _quotes.push_back(q13);
  _quotes.push_back(q14);
  _quotes.push_back(q15);
  _quotes.push_back(q16);
  _quotes.push_back(q17);
  _quotes.push_back(q18);
  _quotes.push_back(q19);
  _quotes.push_back(q20);
  _quotes.push_back(q21);
  _quotes.push_back(q22);
  _quotes.push_back(q23);
  _quotes.push_back(q24);
  _quotes.push_back(q25);
  _quotes.push_back(q26);
}

Quote QuoteManager::getRandomQuote() const
{
  int index = std::rand() % _quotes.size();
  return _quotes[index];
}

bool QuoteManager::isEmpty() const
{
  return _quotes.empty();
}
