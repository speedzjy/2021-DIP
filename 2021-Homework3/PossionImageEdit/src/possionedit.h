#ifndef POSSIONEDIT_H
#define POSSIONEDIT_H

#include <algorithm>
#include <queue>
#include <vector>
#include <iostream>

#include <QImage>
#include <QRgb>
#include <QDebug>
#include <QColor>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Core>
#include <Eigen/SparseCholesky>

class PossionEdit
{
public:
    PossionEdit(){}
    ~PossionEdit(){}
    // set static method to call derictly
    static std::vector<std::vector<bool>> genMask(const QImage &source);
    static QImage forceCloning(const QImage &source, const QImage &background);
    static QImage possionCloning(const QImage &source, const QImage &source_tmp,
                                 const QImage &background, const QImage &background_tmp);
};

#endif // POSSIONEDIT_H
