/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-07-01 23:28:34
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-07-14 23:47:20
 * @FilePath: /svg_lib/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

using namespace std::literals;
using namespace svg;

/*
Пример использования библиотеки. Он будет компилироваться и работать, когда вы реализуете
все классы библиотеки.
*/

namespace {
template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

}  // namespace
namespace shapes {

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable { 
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays){
        polyline_ = CreateStar(center, outer_rad,
                               inner_rad, num_rays); 
    }
     
    void Draw(svg::ObjectContainer& container) const override { 
        container.Add(polyline_);
    }

private:
    svg::Polyline polyline_;
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    
    using namespace svg;
    
    Polyline polyline;
    polyline.SetFillColor("red");
    polyline.SetStrokeColor("black");
    
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    
    return polyline;
}

};
class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point in_base_point, double in_radius):head_base_point_(in_base_point), head_radius_(in_radius){}
    void Draw(svg::ObjectContainer& container) const override { 
        using namespace svg;

        Circle head;
        head.SetFillColor("rgb(240,240,240)");
        head.SetStrokeColor("black");
        head.SetCenter(head_base_point_).SetRadius(head_radius_);

        Circle torso;
        torso.SetFillColor("rgb(240,240,240)");
        torso.SetStrokeColor("black");
        torso.SetCenter({head_base_point_.x, head_base_point_.y + head_radius_*torso_to_head_point_delta}).SetRadius(head_radius_*head_to_torso_radius_ratio_);

        Circle legs;
        legs.SetFillColor("rgb(240,240,240)");
        legs.SetStrokeColor("black");
        legs.SetCenter({head_base_point_.x, head_base_point_.y + head_radius_*legs_to_torso_point_delta+head_radius_*torso_to_head_point_delta}).SetRadius(head_radius_*head_to_legs_radius_ratio_);

        container.Add(legs);
        container.Add(torso);
        container.Add(head);
    }

private:
    svg::Point head_base_point_;
    double head_radius_;
    
    const double head_to_torso_radius_ratio_ = 1.5;
    const double head_to_legs_radius_ratio_  = 2.0;

    const double torso_to_head_point_delta   = 2.0;
    const double legs_to_torso_point_delta   = 3.0;
};
} //end namespace shape  
int main() {
;    using namespace svg;
    using namespace shapes;
    using namespace std;
    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    // Так как документ реализует интерфейс ObjectContainer,
    // его можно передать в DrawPicture в качестве цели для рисования
    DrawPicture(picture, doc);
    const Text base_text =  
        Text()
            .SetFontFamily("Verdana"s)
            .SetFontSize(12)
            .SetPosition({10, 100})
            .SetData("Happy New Year!"s);
    doc.Add(Text{base_text}
                .SetStrokeColor("yellow"s)
                .SetFillColor("yellow"s)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeWidth(3));
    doc.Add(Text{base_text}.SetFillColor("red"s));

    doc.Render(cout);
} 