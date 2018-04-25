//
// Created by Yixin Hu on 3/31/17.
//

#ifndef NEW_GTET_SIMPLETETRAHEDRALIZATION_H
#define NEW_GTET_SIMPLETETRAHEDRALIZATION_H

#include "MeshConformer.h"
#include "TetmeshElements.h"

class SimpleTetrahedralization {
public:
    MeshConformer& MC;
    std::vector<Point_3> centers;

    //useful infos
//    int centroid_size;
    int m_vertices_size;
//    std::vector<bool> is_visited;

    SimpleTetrahedralization(MeshConformer& mc):MC(mc){}
    void tetra(std::vector<TetVertex>& tet_vertices, std::vector<std::array<int, 4>>& tets);
    void triangulation(std::vector<TetVertex>& tet_vertices, std::vector<std::array<int, 4>>& tets);

    void labelSurface(const std::vector<int>& m_f_tags, const std::vector<int>& m_e_tags,
                      const std::vector<std::vector<int>>& conn_e4v,
                      std::vector<TetVertex>& tet_vertices, std::vector<std::array<int, 4>>& tets,
                      std::vector<std::array<int, 4>>& is_surface_fs);
    void labelBbox(std::vector<TetVertex>& tet_vertices, std::vector<std::array<int, 4>>& tets);
    void labelBoundary(std::vector<TetVertex>& tet_vertices, std::vector<std::array<int, 4>>& tets,
                       const std::vector<std::array<int, 4>>& is_surface_fs);

    void constructPlane(int bsp_f_id, Plane_3& pln);
};


#endif //NEW_GTET_SIMPLETETRAHEDRALIZATION_H
