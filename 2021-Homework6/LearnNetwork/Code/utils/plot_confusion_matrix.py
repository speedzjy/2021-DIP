import matplotlib.pyplot as plt
import itertools
import numpy as np


def plot_confusion_matrix(cm, labels_name, title, cmap=plt.cm.Blues):
    plt.figure(figsize=(20, 8))
    # -------------------------------------数字形式的混淆矩阵----------------------------------
    plt.subplot(121)
    plt.imshow(cm, interpolation='nearest', cmap=cmap)  # 在特定的窗口上显示图像
    plt.title(title)  # 图像标题
    plt.colorbar()
    num_local = np.array(range(len(labels_name)))
    plt.xticks(num_local, labels_name, rotation=45)  # 将标签印在x轴坐标上
    plt.yticks(num_local, labels_name)  # 将标签印在y轴坐标上

    fmt = 'd'
    thresh = cm.max() / 2.
    for i, j in itertools.product(range(cm.shape[0]), range(cm.shape[1])):
        plt.text(j, i, format(cm[i, j], fmt),
                 horizontalalignment="center",
                 color="white" if cm[i, j] > thresh else "black")
    plt.tight_layout()
    plt.ylabel('True label')
    plt.xlabel('Predict label')
    # --------------------------------------标准化的混淆矩阵------------------------------------
    plt.subplot(122)
    cm1 = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]
    plt.imshow(cm1, interpolation='nearest', cmap=cmap)  # 在特定的窗口上显示图像
    plt.title('Normalize_' + title)  # 图像标题
    plt.colorbar()
    num_local = np.array(range(len(labels_name)))
    plt.xticks(num_local, labels_name, rotation=45)  # 将标签印在x轴坐标上
    plt.yticks(num_local, labels_name)  # 将标签印在y轴坐标上

    fmt = '.3f'
    thresh = cm1.max() / 2.
    for i, j in itertools.product(range(cm1.shape[0]), range(cm1.shape[1])):
        plt.text(j, i, format(cm1[i, j], fmt),
                 horizontalalignment="center",
                 color="white" if cm1[i, j] > thresh else "black")
    plt.tight_layout()
    plt.ylabel('True label')
    plt.xlabel('Predict label')
    # ---------------------------------------------------------------------------------------------
    plt.show()
