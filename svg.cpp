/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-07-01 23:28:23
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-07-06 23:12:50
 * @FilePath: /svg_lib/svg.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline& Polyline::AddPoint(Point point){
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;

    out << "<polyline points=\""sv;
    if(!points_.empty()){
        std::vector<Point>::const_iterator iterator = points_.begin();

        out << (*iterator).x << ","sv << (*iterator).y;

        while (++iterator != points_.end())
        {
            out << " " << (*iterator).x << ","sv << (*iterator).y;
        }
    }
    
    out << "\" />"sv;

}

// ---------- Text ------------------

Text& Text::SetPosition(Point pos){
    base_point_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset){
    offset_ = offset;
    return *this;
}


Text& Text::SetFontSize(uint32_t size){
    size_ = size;
    return *this;
}


Text& Text::SetFontFamily(std::string font_family){
    font_family_ = font_family;
    return *this;

}


Text& Text::SetFontWeight(std::string font_weight){
    weight_ = font_weight;
    return *this;
}


Text& Text::SetData(std::string data){
    text_ = data;
    return *this;
}

std::string  Text::process_escape_character(const std::string& str)const{
    std::string ret_val;

    for(char symbol : str){
        switch(symbol){
            case '"':
                ret_val += "&quot;"sv;
                break;
            case '\'':
            case '`':
                ret_val += "&apos;"sv;
                break;
            case '<':
                ret_val += "&lt;"sv;
                break;
            case '>':
                ret_val += "&gt;"sv;
                break;
            case '&':
                ret_val += "&amp;"sv;
                break;
            default:
                ret_val += symbol;
                break;
        }
    }

    return ret_val;
}

void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;

        out << "<text x=\""sv
            << base_point_.x << "\" y=\""sv
            << base_point_.y << "\" dx=\""sv
            << offset_.x     << "\" dy=\""sv
            << offset_.y     << "\" font-size=\""sv
            << size_         << "\" "sv;

        
        if (!font_family_.empty()) {
            out << "font-family=\""sv << font_family_ << "\" "sv;
        }
 
        if (!weight_.empty()) {
            out << "font-weight=\"" << weight_ << "\"";
        }
 
        out << ">"sv; 
        out <<  process_escape_character(text_) << "</text>"sv;
}
// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object> &&obj) {
    objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream& out) const{
    int indent = 2;
    int indent_step = 2;
 
    RenderContext context(out, indent_step, indent);
 
    const std::string_view xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    const std::string_view svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
 
    out << xml << "\n"sv << svg << "\n"sv;
 
    for (const auto& object : objects_) {
        object->Render(context);
    }
    
    out << "</svg>"sv;
}

}  //end namespace svg