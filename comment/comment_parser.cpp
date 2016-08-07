#include "comment_parser.h"
#include <pugixml.hpp>
#include <sstream>
#include <locale>
#include <vector>

struct colon_is_space : std::ctype<char> {
    colon_is_space() : std::ctype<char>(get_table()) {}
    static mask const* get_table()
    {
        static mask rc[table_size];
        rc[','] = std::ctype_base::space;
        return &rc[0];
    }
};

namespace Bilibili {

std::queue<CommentBase> comment_parse(std::string filename) {
    std::queue<CommentBase> cbq;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    pugi::xml_node root = doc.child("i");
    for (pugi::xml_node d = root.child("d"); d; d = d.next_sibling()) {
        pugi::xml_attribute p = d.first_attribute();
        std::stringstream pstr(p.value());

        CommentBase base;
        pstr.imbue(std::locale(pstr.getloc(), new colon_is_space));
        int t;
        pstr >> base.time >> t >> base.pt_size >> base.color >> base.timestamp >> base.pool_id >> base.uid_hash >> base.comment_id;
        base.type = static_cast<CommentType>(t);
        base.content = d.text().get();
        cbq.push(base);
    }
    return cbq;
}

}
