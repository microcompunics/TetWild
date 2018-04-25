//
// Created by yihu on 6/13/17.
//

#include "InoutFiltering.h"

#include <igl/winding_number.h>
#include <igl/writeSTL.h>

void InoutFiltering::filter() {
    cout << "In/out filtering..." << endl;

    Eigen::MatrixXd C(std::count(t_is_removed.begin(), t_is_removed.end(), false), 3);
    int cnt = 0;
    for (int i = 0; i < tets.size(); i++) {
        if (t_is_removed[i])
            continue;
        std::vector<Point_3f> vs;
        vs.reserve(4);
        for (int j = 0; j < 4; j++)
            vs.push_back(tet_vertices[tets[i][j]].posf);
        Point_3f p = CGAL::centroid(vs.begin(), vs.end(), CGAL::Dimension_tag<0>());
        for (int j = 0; j < 3; j++)
            C(cnt, j) = p[j];
        cnt++;
    }

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    getSurface(V, F);
    Eigen::VectorXd W;
    igl::winding_number(V, F, C, W);

    std::vector<bool> tmp_t_is_removed = t_is_removed;
    cnt = 0;
    for (int i = 0; i < tets.size(); i++) {
        if (tmp_t_is_removed[i])
            continue;
        tmp_t_is_removed[i] = !(W(cnt) > 0.5);
        cnt++;
    }

    //if the surface is totally reversed
    //TODO: test the correctness
//    if(std::count(tmp_t_is_removed.begin(), tmp_t_is_removed.end(), false)==0) {
//        cout<<"Winding number gives a empty mesh! trying again"<<endl;
//        for (int i = 0; i < F.rows(); i++) {
//            int tmp = F(i, 0);
//            F(i, 0) = F(i, 2);
//            F(i, 2) = F(i, 0);
//        }
//        igl::writeSTL(g_working_dir+g_postfix+"_debug.stl", V, F);
//
//        tmp_t_is_removed = t_is_removed;
//        cnt = 0;
//        for (int i = 0; i < tets.size(); i++) {
//            if (tmp_t_is_removed[i])
//                continue;
//            tmp_t_is_removed[i] = !(W(cnt) > 0.5);
//            cnt++;
//        }
//    }

//    outputWindingNumberField(W);

    t_is_removed = tmp_t_is_removed;

    cout << "In/out Filtered!" << endl;
}

void InoutFiltering::getSurface(Eigen::MatrixXd& V, Eigen::MatrixXi& F){
    std::vector<std::array<int, 3>> fs;
    std::vector<int> vs;
    for(int i=0;i<tets.size();i++) {
        if (t_is_removed[i])
            continue;
        for (int j = 0; j < 4; j++) {
            if (is_surface_fs[i][j] != NOT_SURFACE && is_surface_fs[i][j] > 0) {//outside
                std::array<int, 3> v_ids = {tets[i][(j + 1) % 4], tets[i][(j + 2) % 4], tets[i][(j + 3) % 4]};
                if (CGAL::orientation(tet_vertices[v_ids[0]].pos, tet_vertices[v_ids[1]].pos,
                                      tet_vertices[v_ids[2]].pos, tet_vertices[tets[i][j]].pos) != CGAL::POSITIVE) {
                    int tmp = v_ids[0];
                    v_ids[0] = v_ids[2];
                    v_ids[2] = tmp;
                }
                for (int k = 0; k < is_surface_fs[i][j]; k++)
                    fs.push_back(v_ids);
                for (int k = 0; k < 3; k++)
                    vs.push_back(v_ids[k]);
            }
        }
    }
    std::sort(vs.begin(), vs.end());
    vs.erase(std::unique(vs.begin(), vs.end()), vs.end());

    V.resize(vs.size(), 3);
    std::map<int, int> map_ids;
    for(int i=0;i<vs.size();i++){
        map_ids[vs[i]]=i;
        for(int j=0;j<3;j++)
            V(i, j)=tet_vertices[vs[i]].posf[j];
    }

    F.resize(fs.size(), 3);
    for(int i=0;i<fs.size();i++){
        for(int j=0;j<3;j++)
            F(i, j)=map_ids[fs[i][j]];
    }

//    igl::writeSTL(g_working_dir+g_postfix+"_debug.stl", V, F);
}

void InoutFiltering::outputWindingNumberField(const Eigen::VectorXd& W){
    int t_cnt = W.rows();

    std::vector<int> v_ids;
    for (int i = 0; i < tets.size(); i++) {
        if (t_is_removed[i])
            continue;
        for (int j = 0; j < 4; j++)
            v_ids.push_back(tets[i][j]);
    }
    std::sort(v_ids.begin(), v_ids.end());
    v_ids.erase(std::unique(v_ids.begin(), v_ids.end()), v_ids.end());
    std::unordered_map<int, int> map_ids;
    for (int i = 0; i < v_ids.size(); i++)
        map_ids[v_ids[i]] = i;

    PyMesh::MshSaver mSaver(g_working_dir+g_postfix+"_wn.msh", true);
    Eigen::VectorXd oV(v_ids.size() * 3);
    Eigen::VectorXi oT(t_cnt * 4);
    for (int i = 0; i < v_ids.size(); i++) {
        for (int j = 0; j < 3; j++)
            oV(i * 3 + j) = tet_vertices[v_ids[i]].posf[j];
    }
    int cnt = 0;
    for (int i = 0; i < tets.size(); i++) {
        if (t_is_removed[i])
            continue;
        for (int j = 0; j < 4; j++)
            oT(cnt * 4 + j) = map_ids[tets[i][j]];
        cnt++;
    }
    mSaver.save_mesh(oV, oT, 3, mSaver.TET);
    cout << "#v = " << oV.rows() / 3 << endl;
    cout << "#t = " << oT.rows() / 4 << endl;

    mSaver.save_elem_scalar_field("winding number", W);
}