#include <iostream>
#include "triangulation_2.hpp"
#include "ra/kernel.hpp"
#include "CGAL/Cartesian.h"

using Tri = trilib::Triangulation_2< CGAL::Cartesian<double> >;

int main() {
	ra::geometry::Kernel<double> kern;
	ra::geometry::Kernel<double>::Vector u(1, 0), v(1, 1);

	Tri t(std::cin);
	auto suspect = std::vector<Tri::Halfedge_handle>();

	auto is_in = [&] (auto el) {
		for (auto item : suspect) {
			if (el->edge() == item || el->opposite() == item) {
				return true;
			}
		}

		return false;
	};
	// initially add all edges as suspect
	// only the first half edge is held in the vector
	for (auto i = t.halfedges_begin(); i != t.halfedges_end(); i++, i++) {
		suspect.push_back(i);
	}

	// until there are no more suspect edges
	while (suspect.size() > 0) {
		// loop through all of the suspect edges
		auto e = suspect.begin();
		while (e != suspect.end()) {
			auto edge = (*e);
			auto a = edge->vertex()->point();
			auto b = edge->next()->vertex()->point();
			auto c = edge->opposite()->vertex()->point();
			auto d = edge->opposite()->next()->vertex()->point();
	
			// if it is not flippable
			if (edge->is_border_edge()){
				e = suspect.erase(e);
			}else if(!kern.is_strictly_convex_quad(a, b, c, d)) {
				e = suspect.erase(e);
			} else if (kern.is_locally_pd_delaunay_edge(a, b, c, d, u, v)) {
				e = suspect.erase(e);
			} else {
				auto new_edge = t.flip_edge(edge);
				if (!is_in(new_edge->next())) {
					suspect.push_back(new_edge->next());
				}

				if (!is_in(new_edge->next()->next())) {
					suspect.push_back(new_edge->next()->next());
				}
				
				if (!is_in(new_edge->opposite()->next())) {
					suspect.push_back(new_edge->opposite()->next());
				}
				
				if (!is_in(new_edge->opposite()->next()->next())) {
					suspect.push_back(new_edge->opposite()->next()->next());
				}

				e = suspect.erase(e);
			}
		}
	}

	t.output_off(std::cout);

	return 0;
}
