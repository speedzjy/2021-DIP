import os.path
import torch
import time
from torch.autograd import Variable
from torchvision import transforms
import matplotlib.pyplot as plt
import torch.nn as nn
from sklearn.metrics import confusion_matrix

from utils.net import Net
from utils.plot_confusion_matrix import plot_confusion_matrix
from utils import mnist_dataLoader as mnist_load

root = os.path.join("../mnist")

LR = 0.001
EPOCH = 40
BATCH_SIZE = 128

labels_name = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

# ---------------------------计算设备-----------------------
device = torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")
# ------------------------------------------------------------------------------------------------------

# -------------------------------------------- 读取数据 ------------------------------------------------
train_data = mnist_load.MyDataset(txt=root + '/train.txt', transform=transforms.ToTensor())
test_data = mnist_load.MyDataset(txt=root + '/test.txt', transform=transforms.ToTensor())
train_loader = mnist_load.DataLoader(dataset=train_data, batch_size=BATCH_SIZE, shuffle=True)
test_loader = mnist_load.DataLoader(dataset=test_data, batch_size=BATCH_SIZE)
print(type(train_data))
print(train_loader)

if __name__ == '__main__':

    model = Net()
    model.to(device)
    print(model)
    print("Training in {device}".format(device=device))

    loss_func = nn.CrossEntropyLoss()  # 选择损失函数
    optimizer = torch.optim.Adam(model.parameters(), lr=LR, betas=(0.9, 0.99))  # 选择优化器
    # scheduler = torch.optim.lr_scheduler.StepLR(optimizer, step_size=10, gamma=0.1)  # 设置学习率下降策略

    Acc = [[], []]
    Loss = [[], []]

    for epoch in range(EPOCH):
        start_time = time.time()

        print('#-------epoch {}--------'.format(epoch + 1))
        print('learning rate = ', optimizer.param_groups[0]['lr'])

        # ----------------------------训练模式-------------------------------
        model.train()

        train_loss, train_acc = 0., 0.
        # ---------X,y = imgs, labels---------------
        for batch_x, batch_y in train_loader:
            batch_x = batch_x.to(device)
            batch_y = batch_y.to(device)
            batch_x, batch_y = Variable(batch_x), Variable(batch_y)
            out = model(batch_x)
            # --------------计算损失函数，梯度下降，更新参数--------------
            loss = loss_func(out, batch_y)
            optimizer.zero_grad()  # 清空梯度
            loss.backward()
            optimizer.step()  # 更新参数
            # ------------计算损失，准确率----------------
            train_acc += (torch.max(out, 1)[1] == batch_y).sum().cpu().item()
            train_loss += loss.cpu().item()

        total_loss = (train_loss * float(BATCH_SIZE)) / len(train_data)
        total_acc = train_acc / (len(train_data))
        Acc[0].append(total_acc)
        Loss[0].append(total_loss)
        print('Train Loss: {:.6f}, Acc: {:.6f}'.format(total_loss, total_acc))

        # ----------------------------------测试模式---------------------------------------
        model.eval()
        eval_loss = 0.
        eval_acc = 0.

        # ---------------------------------batch_x, batch_y  图像  标签----------------------------------
        y_true = []
        y_pred = []
        image = []
        # -------------------------------------------------------------------------------------------------
        with torch.no_grad():
            for batch_x, batch_y in test_loader:
                batch_x = batch_x.to(device)
                batch_y = batch_y.to(device)
                batch_x, batch_y = Variable(batch_x), Variable(batch_y)
                out = model(batch_x)
                # --------------计算损失, 准确率----------
                loss = loss_func(out, batch_y)
                eval_loss += loss.cpu().item()
                # --------------------真实值和预测值转化为数组，构造混淆矩阵--------------
                y_true.extend(batch_y.cpu().numpy().tolist())
                y_pred.extend(torch.max(out, 1)[1].cpu().numpy().tolist())
                image.extend(batch_x.cpu().numpy().tolist())
                # -----------------------------------------------------------------------

                num_correct = (torch.max(out, 1)[1] == batch_y).sum()
                eval_acc += num_correct.cpu().item()

            # ------------------------------------绘制混淆矩阵------------------------------
            if (epoch + 1) == EPOCH:
                print(y_true)
                print(y_pred)
                for i in range(len(y_pred)):
                    if y_true[i] != y_pred[i]:
                        print("The {i}th image fail recognized".format(i=i))
                        print("true: {y_true}  pred: {y_pred}".format(y_true=y_true[i], y_pred=y_pred[i]))
                        plt.imshow(image[i][0])
                        plt.show()
                        break
                print(confusion_matrix(y_true, y_pred))
                mtx = confusion_matrix(y_true, y_pred)
                plot_confusion_matrix(mtx, labels_name, 'Confusion_Matrix')
            # -------------------------------------------------------------------------------
            total_loss = (eval_loss * float(BATCH_SIZE)) / len(test_data)
            total_acc = eval_acc / (len(test_data))
            Acc[1].append(total_acc)
            Loss[1].append(total_loss)
            print('Test Loss: {:.6f}, Acc: {:.6f}'.format(total_loss, total_acc))
            print('training took %fs!' % (time.time() - start_time))

            # scheduler.step()  # 更新学习率

    # torch.save(Net, 'fashion_mnist_module.pkl' + str(epoch))

    labels = ['train-acc', 'test-acc']
    colors = ['red', 'purple']
    styles = ["x", "+"]
    plt.subplot(121)
    k = 0
    for i, acc in enumerate(Acc):
        plt.plot(acc, label=labels[i], marker=styles[k], color=colors[k], linewidth=2)
        k = k + 1
    plt.title('acc----LR: ' + str(LR) + '----batchsize: ' + str(BATCH_SIZE))

    plt.legend(loc='best')
    plt.xlabel('Epoch')
    plt.ylabel('Acc')
    # plt.show()

    labels = ['train-loss', 'test-loss']
    plt.subplot(122)
    k = 0
    for i, loss in enumerate(Loss):
        plt.plot(loss, label=labels[i], marker=styles[k], color=colors[k], linewidth=2)
        k = k + 1
    plt.title('loss----LR: ' + str(LR) + '----batchsize: ' + str(BATCH_SIZE))
    plt.legend(loc='best')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()
