#include "view.h"
#include "tags.h"
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>

#include "../app/use_cases.h"
#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
namespace detail {

std::ostream& operator<<(std::ostream& out, const AuthorInfo& author) {
    out << author.name;
    return out;
}

std::ostream& operator<<(std::ostream& out, const BookInfo& book) {
    out << book.title << ", " << book.publication_year;
    return out;
}

}  // namespace detail

template <typename T>
void PrintVector(std::ostream& out, const std::vector<T>& vector) {
    int i = 1;
    for (auto& value : vector) {
        out << i++ << " " << value << std::endl;
    }
}

void PrintShowBooks(std::ostream& out, const std::vector<ui::detail::BookInfo>& vector) {
    
    std::vector<ui::detail::BookInfo> unsortedVec = vector;

    auto rules = [](const auto& lhs, const auto& rhs) {
        if(lhs.title != rhs.title) return rhs.title > lhs.title;
        if(lhs.author_name != rhs.author_name) return rhs.author_name > lhs.author_name;
        return rhs.publication_year > lhs.publication_year;
        };
    std::sort(unsortedVec.begin(), unsortedVec.end(), rules);
    int i = 1;
    for (auto& value : vector) {
        out << i++ << " " << value.title << " by " << value.author_name << ", " << value.publication_year << std::endl;
    }
}

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}
    , use_cases_{use_cases}
    , input_{input}
    , output_{output} {
    menu_.AddAction(  //
        "AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
        // ����
        // [this](auto& cmd_input) { return AddAuthor(cmd_input); }
    );
    menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s,
                    std::bind(&View::AddBook, this, ph::_1));
    menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
    menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
    menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s,
                    std::bind(&View::ShowAuthorBooks, this));

    menu_.AddAction("DeleteAuthor"s, "name"s, "DeleteAuthor"s, std::bind(&View::DeleteAutor, this, ph::_1));
    menu_.AddAction("EditAuthor"s, "name"s, "EditAuthor"s, std::bind(&View::EditAuthor, this, ph::_1));
    menu_.AddAction("ShowBook"s, "name"s, "ShowBook"s, std::bind(&View::ShowBook, this, ph::_1));
    menu_.AddAction("DeleteBook"s, "name"s, "DeleteBook"s, std::bind(&View::DeleteBook, this, ph::_1));
    menu_.AddAction("EditBook"s, "name"s, "EditBook"s, std::bind(&View::EditBook, this, ph::_1));
}

bool View::AddAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);
        if (name.empty()) {
            throw std::invalid_argument("Empty name");
        }
        use_cases_.AddAuthor(std::move(name));
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }
    return true;
}

bool View::AddBook(std::istream& cmd_input) const {
    try {
        if (auto params = GetBookParams(cmd_input)) {
            //assert(!"TODO: implement book adding");
            use_cases_.AddBook(params->author_id, params->title, params->publication_year);
            AddBookTags(params->title);
        }
    } catch (const std::exception& e) {
        output_ << "Failed to add book"sv << e.what() << std::endl;
    }
    return true;
}

bool View::ShowAuthors() const {

    for (auto& author : GetAuthors()) {
        output_ << author << std::endl;
    }
    return true;
}

bool View::ShowAuthorsWithId() const {

    PrintVector(output_, GetAuthors());
    return true;
}

bool View::ShowBooks() const {
    PrintShowBooks(output_, GetBooks());
    return true;
}

bool View::ShowAuthorBooks() const {
    // TODO: handle error
    try {
        if (auto author_id = SelectAuthor()) {
            PrintVector(output_, GetAuthorBooks(*author_id));
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to Show Books");
    }
    return true;
}

std::optional<detail::AddBookParams> View::GetBookParams(std::istream& cmd_input) const {
    detail::AddBookParams params;

    cmd_input >> params.publication_year;
    std::getline(cmd_input, params.title);
    boost::algorithm::trim(params.title);

    auto author_id = SelectAuthor();
    
    if (not author_id.has_value())
        return std::nullopt;
    else {
        params.author_id = author_id.value();
        return params;
    }

    

}

std::optional<std::string> View::SelectAuthor() const {
    auto authors = GetAuthors();
    output_ << "Enter author name or empty line to select from list:" << std::endl;

    std::string authorName;
    if (!std::getline(input_, authorName) || authorName.empty()) {
        ShowAuthors();
        std::getline(input_, authorName);
    }
    
    auto currentAuthors = use_cases_.GetAuthors();
    auto equalsAuthor = [&authorName](const domain::Author& author) {return author.GetName() == authorName; };

    if(std::find_if(currentAuthors.begin(), currentAuthors.end(), equalsAuthor) == currentAuthors.end()) {      //Не найден автор
        output_ << "No author found. Do you want to add " << authorName << " (y / n) ? " << std::endl;
        std::string ynAnswer;
        if (!std::getline(input_, ynAnswer) || ynAnswer.empty() || (ynAnswer != "Y" && ynAnswer != "y")) {
            throw std::invalid_argument("");            //Нет положительного ответа
            return std::nullopt;
        }
        else {
            use_cases_.AddAuthor(authorName);           //Добавили автора, который задал пользователь
        }
    }
    else {//автор выбран из списка

    }
    
    return GetAuthorByName(authorName).id;
}

std::vector<detail::AuthorInfo> View::GetAuthors() const {
    std::vector<detail::AuthorInfo> tmpAuthors;
    //assert(!"TODO: implement GetAuthors()");
    auto authorsFromDB = use_cases_.GetAuthors();

    for (auto author : authorsFromDB) {
        tmpAuthors.emplace_back(detail::AuthorInfo{ author.GetId().ToString(), author.GetName() });
    }

    return tmpAuthors;


}

std::vector<detail::BookInfo> View::GetBooks() const {
    std::vector<detail::BookInfo> tmpBooks;
    //assert(!"TODO: implement GetBooks()");
    auto booksFromDB = use_cases_.GetBooks();

    for (auto book : booksFromDB) {
        tmpBooks.emplace_back(detail::BookInfo{ book.GetTitle(), book.GetPublicationYear(), GetNameByAuthorId(book.GetAuthorId().ToString()),book.GetBookId().ToString()});
    }

    return tmpBooks;
}

std::vector<detail::BookInfo> View::GetAuthorBooks(const std::string& author_id) const {
    std::vector<detail::BookInfo> tmpBooks;
    //assert(!"TODO: implement GetAuthorBooks()");
    auto booksFromDB = use_cases_.GetBooksByAuthorId(author_id);

    for (auto book : booksFromDB) {
        tmpBooks.emplace_back(detail::BookInfo{ book.GetTitle(), book.GetPublicationYear()});
    }

    return tmpBooks;
}


ui::detail::AuthorInfo View::GetAuthorByName(const std::string& authorName) const {

    auto currentAuthors = GetAuthors();
    auto equalsAuthor = [&authorName](const ui::detail::AuthorInfo& author) {return author.name == authorName; };

    auto authorItr = std::find_if(currentAuthors.begin(), currentAuthors.end(), equalsAuthor);
    if (authorItr == currentAuthors.end()) {
        throw std::invalid_argument("");
    }
    return *authorItr;
}

void View::AddBookTags(std::string book_name) const{
    output_ << "Enter tags (comma separated):" << std::endl;
    std::string rowTags;
    std::getline(input_, rowTags);
    if (rowTags.empty())return;
    auto Tags = SplitStringByChar(rowTags, ',');

    auto books = use_cases_.GetBooks();
    auto equalsBook = [&book_name](const domain::Book& book) {return book.GetTitle() == book_name; };
    auto bookItr = std::find_if(books.begin(), books.end(), equalsBook);
    for (auto tag : Tags) {
        use_cases_.AddTags(bookItr->GetBookId(), tag);
    }
    
}

std::set<std::string> View::SplitStringByChar(std::string string, char ch) const {
    std::set<std::string> tmpVector;
    std::string tmpStr;
    for (auto strCh: string) {
        
        if (strCh == ch) {
            if (tmpStr.empty()) {
                continue;
            }

            size_t strBegin = tmpStr.find_first_not_of(' ');
            size_t strEnd = tmpStr.find_last_not_of(' ');

            tmpStr.erase(strEnd + 1, string.size() - strEnd);
            tmpStr.erase(0, strBegin);

            tmpVector.insert(tmpStr);
            tmpStr.erase();
        }
        else {
            tmpStr += strCh;
        }
    }

    size_t strBegin = tmpStr.find_first_not_of(' ');
    size_t strEnd = tmpStr.find_last_not_of(' ');

    tmpStr.erase(strEnd + 1, string.size() - strEnd);
    tmpStr.erase(0, strBegin);

    tmpVector.insert(tmpStr);

    return tmpVector;
}



bool View::DeleteAutor(std::istream& cmd_input) const {

    try {
        std::string authorName;
        std::string authorId;
        std::string authorIdSalt;
        auto currentAuthors = GetAuthors();
        if (!std::getline(cmd_input, authorName) || authorName.empty()) {
            ShowAuthorsWithId();
            output_ << "Enter author # or empty line to cancel" << std::endl;
           
            std::getline(input_, authorId);

            if (authorId.empty()) return false;

            int author_idx;
            try {
                author_idx = std::stoi(authorId);
            }
            catch (std::exception const&) {
                throw std::runtime_error("Invalid author num");
            }

            --author_idx;
            if (author_idx < 0 or author_idx >= currentAuthors.size()) {
                throw std::runtime_error("Invalid author num");
            }

            authorIdSalt = currentAuthors[author_idx].id;
            authorName = use_cases_.GetAuthorsByAuthorId(authorIdSalt).GetName();
        }
        size_t strBegin = authorName.find_first_not_of(' ');
        authorName.erase(0, strBegin);
        auto author = GetAuthorByName(authorName);
        DeleteBooksByAuthorId(author.id);
        use_cases_.DeleteAuthor(author.id, author.name); //В самую последнюю очередь, т.к. с айди связаны книги и теги
    }
    catch (const std::exception& e) {
        output_ << "Failed to delete author"sv << std::endl;
    }

    return true;
}

bool View::DeleteBooksByAuthorId(const std::string& author_id) const {
    auto booksByAuthor = use_cases_.GetBooksByAuthorId(author_id);
    for (auto book : booksByAuthor) {
        DeleteTagsByBookId(book.GetBookId().ToString());
        //Удалить книгу
        use_cases_.DeleteBook(book);
    }

    return true;
}

bool View::DeleteTagsByBookId(const std::string& book_id) const {
    domain::BookId BookId = domain::BookId::FromString(book_id);

    auto tagsByBook = use_cases_.GetTagsByBookId(BookId);
    for (auto tag : tagsByBook) {
        use_cases_.DeleteTag(tag);
    }
    return true;
}

bool View::EditAuthor(std::istream& cmd_input) const {
    try {
        std::string authorName;
        std::string authorNewName;
        std::string authorId;
        std::string authorIdSalt;
        auto currentAuthors = GetAuthors();
        if (!std::getline(cmd_input, authorName) || authorName.empty()) {
            ShowAuthorsWithId();
            output_ << "Enter author # or empty line to cancel" << std::endl;

            std::getline(input_, authorId);

            if (authorId.empty()) return true;

            int author_idx;
            try {
                author_idx = std::stoi(authorId);
            }
            catch (std::exception const&) {
                throw std::runtime_error("Invalid author num");
            }

            --author_idx;
            if (author_idx < 0 or author_idx >= currentAuthors.size()) {
                throw std::runtime_error("Invalid author num");
            }

            authorIdSalt = currentAuthors[author_idx].id;
            authorName = use_cases_.GetAuthorsByAuthorId(authorIdSalt).GetName();
        }

        size_t strBegin = authorName.find_first_not_of(' ');
        authorName.erase(0, strBegin);
        auto author = GetAuthorByName(authorName);

        output_ << "Enter new name:" << std::endl;
        std::getline(input_, authorNewName);
        use_cases_.EditAuthor(domain::Author{ domain::AuthorId::FromString(author.id), author.name }, authorNewName);
    }
    catch (const std::exception& e) {
        output_ << "Failed to edit author"sv << std::endl;
    }

    return true;
}

std::string View::GetNameByAuthorId(const std::string& author_id) const {
    auto currentAuthors = GetAuthors();

    auto equalsAuthor = [&author_id](const ui::detail::AuthorInfo& author) {return author.id == author_id; };

    auto authorItr = std::find_if(currentAuthors.begin(), currentAuthors.end(), equalsAuthor);
    if (authorItr == currentAuthors.end()) {
        throw std::invalid_argument("");
    }
    return authorItr->name;
}

std::vector<ui::detail::BookInfo> View::GetBookByName(const std::string& bookName) const {

    auto currentBook = GetBooks();
    std::vector<ui::detail::BookInfo> tmpVec;
    for (auto book : currentBook) {
        if (book.title == bookName){
            tmpVec.push_back(book);
        }
    }
    return tmpVec;
}

bool View::ShowBook(std::istream& cmd_input) const {
    std::vector<ui::detail::BookInfo> booksByName;
    ui::detail::BookInfo book;
    std::string bookName;
    std::string bookId;

    auto currentBooks = GetBooks();
    if (!std::getline(cmd_input, bookName) || bookName.empty()) {
        PrintShowBooks(output_, currentBooks);
        output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
        std::getline(input_, bookId);
        if (bookId.empty()) return true;
        book = currentBooks[std::stoi(bookId) - 1];
    }
    else {
        size_t strBegin = bookName.find_first_not_of(' ');
        bookName.erase(0, strBegin);
        booksByName = GetBookByName(bookName);
        if (booksByName.empty()) {
            return true;
        }

        if (booksByName.size() == 1) {
            book = booksByName[0];
        }
        else {
            PrintShowBooks(output_, booksByName);
            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            std::getline(input_, bookId);
            if (bookId.empty()) return true;
            book = booksByName[std::stoi(bookId) - 1];
        }
    }
    PrintAllAboutABook(book);
    return true;
}

//std::string title;
//int publication_year;
//std::string author_name;
void View::PrintAllAboutABook(ui::detail::BookInfo book) const{
    std::set < std::string> tags;
    output_ << "Title: " << book.title << std::endl;
    output_ << "Author: " << book.author_name << std::endl;
    output_ << "Publication year: " << book.publication_year << std::endl;
    tags = use_cases_.GetTagsByBookId(domain::BookId::FromString(book.book_id));
    if (!tags.empty()) {
        output_ << "Tags: ";
        bool isFirst = true;
        for (auto tag : tags) {
            if (isFirst) {
                isFirst = false;
                output_ << tag;
            }
            else {
                output_ << ", " << tag;
            }
        }
        output_ << std::endl;

    }
}

bool View::DeleteBook(std::istream& cmd_input) const {
    try {

        std::vector<ui::detail::BookInfo> booksByName;
        ui::detail::BookInfo book;
        std::string bookName;
        std::string bookId;

        auto currentBooks = GetBooks();
        if (!std::getline(cmd_input, bookName) || bookName.empty()) {
            PrintShowBooks(output_, currentBooks);
            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            std::getline(input_, bookId);
            if (bookId.empty()) return true;
            book = currentBooks[std::stoi(bookId) - 1];
        }
        else {
            size_t strBegin = bookName.find_first_not_of(' ');
            bookName.erase(0, strBegin);
            booksByName = GetBookByName(bookName);
            if (booksByName.empty()) {
                return true;
            }

            if (booksByName.size() == 1) {
                book = booksByName[0];
            }
            else {
                PrintShowBooks(output_, booksByName);
                output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
                std::getline(input_, bookId);
                if (bookId.empty()) return true;
                book = booksByName[std::stoi(bookId) - 1];
            }
        }
        //BookId book_id, AuthorId author_id, std::string title, int year
        DeleteTagsByBookId(book.book_id);
        use_cases_.DeleteBook(domain::Book{domain::BookId::FromString(book.book_id), 
            domain::AuthorId::FromString(GetAuthorByName(book.author_name).id),
            book.title,
            book.publication_year
            });
    }
    catch (const std::exception& e) {
        output_ << "Failed to delete book"sv << std::endl;
    }
}

bool View::EditBook(std::istream& cmd_input) const {
    try {
        std::vector<ui::detail::BookInfo> booksByName;
        ui::detail::BookInfo book;
        std::string bookName;
        std::string bookNewName;
        std::string bookId;

        auto currentBooks = GetBooks();
        if (!std::getline(cmd_input, bookName) || bookName.empty()) {
            PrintShowBooks(output_, currentBooks);
            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            std::getline(input_, bookId);
            if (bookId.empty()) return true;
            book = currentBooks[std::stoi(bookId) - 1];
        }
        else {
            size_t strBegin = bookName.find_first_not_of(' ');
            bookName.erase(0, strBegin);
            booksByName = GetBookByName(bookName);
            if (booksByName.empty()) {
                output_ << "Book not found"sv << std::endl;
                return true;
            }

            if (booksByName.size() == 1) {
                book = booksByName[0];
            }
            else {
                PrintShowBooks(output_, booksByName);
                output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
                std::getline(input_, bookId);
                if (bookId.empty()) return true;
                book = booksByName[std::stoi(bookId) - 1];
            }
        }

        output_ << "Enter new title or empty line to use the current one (" << book.title << "):" << std::endl;
        std::getline(input_, bookNewName);
        //(BookId book_id, AuthorId author_id, std::string title, int year)
        if (!bookNewName.empty()) {
            use_cases_.EditBook(domain::Book{ domain::BookId::FromString(book.book_id),
                        domain::AuthorId::FromString(GetAuthorByName(book.author_name).id),
                        book.title,
                        book.publication_year
                        }, bookNewName);
        }

        std::string bookNewYear;
        output_ << "Enter publication year or empty line to use the current one (" << book.publication_year << "):" << std::endl;
        std::getline(input_, bookNewYear);
        if (!bookNewYear.empty()) {
            //новая дата книги
            use_cases_.EditBookYear(domain::BookId::FromString(book.book_id), bookNewYear);
        }

        output_ << "Enter tags (current tags: ";  
        bool isFirst = true;
        for (auto tag : use_cases_.GetTagsByBookId(domain::BookId::FromString(book.book_id))) {
            if (isFirst) {
                output_ << tag;
                isFirst = false;
            }
            else {
                output_ << ", " << tag;
            }
        }
        output_ << "):" << std::endl;

        std::string rowTags;
        std::getline(input_, rowTags);
        if (rowTags.empty()) return true;
        auto Tags = SplitStringByChar(rowTags, ',');

        use_cases_.DeleteAllTagsByBook(domain::Book{ domain::BookId::FromString(book.book_id),
            domain::AuthorId::FromString(GetAuthorByName(book.author_name).id),
            book.title,
            book.publication_year });

        for (auto tag :Tags) {
            use_cases_.AddTags(domain::BookId::FromString(book.book_id), tag);
        }
        

    }
    catch (const std::exception& e) {
        output_ << "Failed to edit book"sv << std::endl;
    }

    return true;
}

}  // namespace ui