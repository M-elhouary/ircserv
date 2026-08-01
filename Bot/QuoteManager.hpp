#ifndef QUOTEMANAGER_HPP
# define QUOTEMANAGER_HPP

#include <string>
#include <vector>

struct Quote
{
    std::string category;
    std::string text;
    std::string author;
};

class QuoteManager
{
  private:
    std::vector<Quote> _quotes;
    QuoteManager(const QuoteManager&);
    QuoteManager& operator=(const QuoteManager&);

  public:
    QuoteManager();
    ~QuoteManager();
    void loadQuotes();
    Quote getRandomQuote() const;
    bool isEmpty() const;
};

#endif
