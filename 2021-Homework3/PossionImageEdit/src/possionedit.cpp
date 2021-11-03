#include "possionedit.h"

using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::BiCGSTAB;
using std::vector;

const int dx[] = {-1, 0, 0, 1};
const int dy[] = {0, -1, 1, 0};

std::vector<std::vector<bool>> PossionEdit::genMask(const QImage &source)
{
    std::queue<int> qx, qy;
    std::vector<std::vector<bool>> noted(source.width(), std::vector<bool>(source.height()));

    for(int i = 0; i < source.width(); ++i)
        for(int j = 0; j < source.height(); ++j)
            if(source.pixel(i, j) == qRgb(255, 0, 0))
                noted[i][j] = true;

    // placeholder for the next while cycle
    qx.push(0);
    qy.push(0);

    // BFS, get mask
    // the area drawn by the painter is left blank
    while(!qx.empty() && !qy.empty()) {
        int sx = qx.front(), sy = qy.front();
        qx.pop();
        qy.pop();
        for (int k = 0; k < 4; ++k) {
            int cur_x = sx + dx[k], cur_y = sy + dy[k];
            if (cur_x >= 0 && cur_x < source.width() &&
                cur_y >= 0 && cur_y < source.height() &&
                !noted[cur_x][cur_y]) {
                noted[cur_x][cur_y] = true;
                qx.push(cur_x);
                qy.push(cur_y);
            }
        }
    }

    return noted;
}

QImage PossionEdit::forceCloning(const QImage &source, const QImage &background)
{
    auto mask = genMask(source);
    QImage result(background.copy());
    for(int i = 0; i < std::min(background.width(), source.width()); ++i)
        for(int j = 0;j < std::min(background.height(), source.height()); ++j)
            if(!mask[i][j])
                result.setPixel(i, j, source.pixel(i,j));
    return result;
}

QImage PossionEdit::possionCloning(const QImage &source, const QImage &source_tmp,
                                   const QImage &background, const QImage &background_tmp)
{
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    auto mask = genMask(source_tmp);

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_mask = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Gen Mask take: " << time_mask.count() << "seconds" << std::endl;

    // Count the number of
    // points in the foreground area

    std::vector<std::vector<int>> cnumber(background.width(), vector<int>(background.height()));
    int count = 0;
    for(int i = 0; i < std::min(background.width(), source.width()); ++i)
        for(int j = 0; j < std::min(background.height(), source.height()); ++j)
           if(!mask[i][j])
               cnumber[i][j] = count++;

    std::cout << "Circle area contains " << count << "points." <<std::endl;


    // search for backend flag points
    int diff_x = 0, diff_y = 0;
    int sta_x = 0, sta_y = 0, end_x =0, end_y = 0;
    for(int i = 0; i < background_tmp.width(); ++i)
        for(int j = 0; j < background_tmp.height(); ++j) {
            if(background_tmp.pixel(i, j) == qRgb(255, 0, 0)) {
                sta_x = i;
                sta_y = j;
            }
            if(background_tmp.pixel(i, j) == qRgb(128, 0, 128)) {
                end_x = i;
                end_y = j;
            }
        }

    diff_x = end_x - sta_x;
    diff_y = end_y - sta_y;

    qDebug() << "Displacement vector: " << diff_x << " " << diff_y << endl;
    // Ax=b
    // x: every point in the foreground area
    VectorXd result[3] = {VectorXd(count), VectorXd(count), VectorXd(count)};
    VectorXd b[3] = {VectorXd(count), VectorXd(count), VectorXd(count)};
    SparseMatrix<double> A(count, count);


    // BFS search neighbor
    for(int i = 0; i < std::min(background.width(), source.width()); ++i)
        for(int j = 0; j < std::min(background.height(), source.height()); ++j) {
            if (!mask[i][j]){
                int p_neighbors = 0;

                QRgb tmp_bkd_pixel = background.pixel(i+diff_x, j+diff_y);
                QRgb tmp_src_pixel = source.pixel(i, j);
                int pij_bkd[3] = {qRed(tmp_bkd_pixel), qGreen(tmp_bkd_pixel), qBlue(tmp_bkd_pixel)};
                int pij_src[3] = {qRed(tmp_src_pixel), qGreen(tmp_src_pixel), qBlue(tmp_src_pixel)};

                for(int k = 0; k < 3; ++k)
                    b[k](cnumber[i][j]) = 0;

                // neighbor_x, neighbor_y
                for(int u = 0; u < 4; ++u)
                {
                    int nx = i + dx[u], ny = j + dy[u];

                    if (nx >= 0 && nx < std::min(background.width(), source.width()) &&
                        ny >= 0 && ny < std::min(background.height(), source.height()))
                    {
                       p_neighbors++;

                       tmp_bkd_pixel = background.pixel(nx+diff_x, ny+diff_y);
                       tmp_src_pixel = source.pixel(nx, ny);
                       int qxy_bkd[3] = {qRed(tmp_bkd_pixel), qGreen(tmp_bkd_pixel), qBlue(tmp_bkd_pixel)};
                       int qxy_src[3] = {qRed(tmp_src_pixel), qGreen(tmp_src_pixel), qBlue(tmp_src_pixel)};

                       if (!mask[nx][ny]){
                           // p, p_neighbor both in interior of omiga
                           // Np * fp - \sigma fq = \sigma Vpq
                           A.insert(cnumber[i][j], cnumber[nx][ny]) = -1;
                       } else {
                           // p in boundary of omiga
                           // Np * fp - \sigma f(q) = \sigma f*(q) + \sigma Vpq
                           // in this case f(q) = f*(q)
                           // set b as background pixel
                           for(int k = 0; k < 3; ++k)
                               b[k](cnumber[i][j]) += qxy_bkd[k];
                       }

                       // mixing gradients, choose max
                       for(int k = 0; k < 3; ++k)
                           if(std::abs(pij_bkd[k] - qxy_bkd[k]) > std::abs(pij_src[k] - qxy_src[k]))
                               b[k](cnumber[i][j]) += pij_bkd[k] - qxy_bkd[k];
                           else
                               b[k](cnumber[i][j]) += pij_src[k] - qxy_src[k];
                    }
                }
                A.insert(cnumber[i][j], cnumber[i][j]) = p_neighbors;
            }
        }

    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_sparse = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2);
    std::cout << "Gen Sparse Matrix take: " << time_sparse.count() << "seconds" << std::endl;

    Eigen::SimplicialLLT<SparseMatrix<double>> solver;
    solver.compute(A);

    qDebug() << "Solve..." << endl;
    for(int k = 0; k < 3; ++k){
        result[k] = solver.solve(b[k]);
    }

    std::chrono::steady_clock::time_point t4 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_solve = std::chrono::duration_cast<std::chrono::duration<double>>(t4 - t3);
    std::cout << "Solve Sparse Matrix take: " << time_solve.count() << "seconds" << std::endl;

    QImage possionCloneImg(background.copy());

    for(int i = 0; i < std::min(background.width(), source.width()); ++i)
        for(int j = 0; j < std::min(background.height(), source.height()); ++j) {
            if (!mask[i][j]) {
                uint8_t pixel[3];
                for(int k = 0;k < 3; ++k){
                    double t = result[k](cnumber[i][j]);
                    pixel[k] =  t > 255 ? 255 : (t < 0 ? 0 : uint8_t(t));
                }
                possionCloneImg.setPixel(i+diff_x, j+diff_y, qRgb(pixel[0], pixel[1], pixel[2]));
            }
        }

    std::chrono::steady_clock::time_point t5 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_gen_res = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t4);
    std::cout << "Gen result picture take: " << time_gen_res.count() << "seconds" << std::endl;

    std::chrono::duration<double> time_total = std::chrono::duration_cast<std::chrono::duration<double>>(t5 - t1);
    std::cout << "Total take: " << time_total.count() << "seconds" << std::endl;

    return possionCloneImg;
}
