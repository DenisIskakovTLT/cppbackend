// PostgresSQL.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <cstdlib>
#include <iostream>
#include <optional>

#include <boost/json/object.hpp>
#include <boost/json/src.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/value.hpp>

#include <pqxx/pqxx>

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

struct Book {
    std::string title;
    std::string author;
    int year;
    std::string ISBN;
};

struct Request {
    std::string action;
    boost::json::value payload;
};

struct Res {
    std::string res;
};

void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, Res const& res) {
    boost::json::object obj;
    obj["result"] = res.res;
    jv.emplace_object() = obj;
}

Request ParseRequest(std::string str) {
    // Распарсить строку как JSON, используя boost::json::parse
    // Получаем json-объект из строки (тип value)
    auto inputJson = boost::json::parse(str);
    auto obj = inputJson.as_object();

    return {
        value_to<std::string>(obj.at(std::string("action"))),
        obj.at(std::string("payload"))
    };
}

Book ParsePayload(const boost::json::value& val) {
    auto obj = val.as_object();

    auto ISBNfromJson = obj.at(std::string("ISBN"));

    std::string tmpIBSN;
    if (ISBNfromJson.is_string()) {
        tmpIBSN = ISBNfromJson.as_string();
    }
    else {
        tmpIBSN = "null";
    }

    return {
        value_to<std::string>(obj.at(std::string("title"))),
        value_to<std::string>(obj.at(std::string("author"))),
        value_to<int>(obj.at(std::string("year"))),
        tmpIBSN
    };
}

boost::json::object AddBook(pqxx::connection& conn, const boost::json::value& payload) {
    boost::json::object tmpRes;
    Book tmpBook;

    try {
        tmpBook = ParsePayload(payload);
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        tmpRes["result"] = "false";
        return tmpRes;
    }

    try {
        
        pqxx::work w(conn);
        std::optional<std::string> ISBN = std::nullopt;
        if (tmpBook.ISBN != "null") {
            ISBN = tmpBook.ISBN;
        }
        w.exec_prepared("add_book"_zv, tmpBook.title, tmpBook.author, tmpBook.year, ISBN);
        w.commit();
    }
    catch (const std::exception& e) {
        tmpRes["result"] = "false";
        return tmpRes;
    }

    tmpRes["result"] = "true";
    return tmpRes;
}

boost::json::array OutAllBooks(pqxx::connection& conn) {

    auto query = "SELECT * FROM books ORDER BY year DESC, title, author, ISBN"_zv;
    
    pqxx::read_transaction r(conn);

    boost::json::array jsonArray;

    
    for (auto [id, title, author, year, ISBN]
        : r.query<int, std::string_view, std::string_view, int, std::optional<std::string>>(query))
    {
        boost::json::object obj;
        obj["id"] = id;
        obj["title"] = title;
        obj["author"] = author;
        obj["year"] = year;
        obj["ISBN"];
        if (ISBN) {
            obj["ISBN"] = *ISBN;
        }
        jsonArray.push_back(obj);
    }

    return jsonArray;
}

int main(int argc, const char* argv[])
{
    try {
        if (argc == 1) {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return EXIT_SUCCESS;
        }
        else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }


        pqxx::connection conn{ argv[1] };

        pqxx::work w(conn);
        
        w.exec(
            "CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY, title varchar(100) NOT NULL, author varchar(100) NOT NULL, year integer NOT NULL, ISBN varchar(13) UNIQUE);"_zv);

        w.commit();

        conn.prepare("add_book"_zv, "INSERT INTO books (title, author, year, ISBN) VALUES ($1, $2, $3, $4)"_zv);
        conn.prepare("all_books"_zv, "SELECT * FROM books;"_zv);

        while (true) {

            std::string jsonRequests;
            std::getline(std::cin, jsonRequests);
            Request request;
            try {
                request = ParseRequest(jsonRequests);
            }
            catch (const std::exception& e) {
                std::cerr << "JSON not parse: " << e.what() << std::endl;
                return EXIT_FAILURE;
            }


            if (request.action == "exit") {
                break;
            }
            else if (request.action == "add_book") {
                std::cout << AddBook(conn, request.payload) << std::endl;
            }
            else if (request.action == "all_books") {
                std::cout << OutAllBooks(conn) << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
