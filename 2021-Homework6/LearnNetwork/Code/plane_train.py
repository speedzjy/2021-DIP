import os.path
import torch
import time
from torch.autograd import Variable
from torchvision import transforms
import matplotlib.pyplot as plt
import torch.nn as nn
from sklearn.metrics import confusion_matrix

from utils.plot_confusion_matrix import plot_confusion_matrix
from utils.points_net import PlaneNet
import utils.points_dataloader as points_load

root = os.path.join("../data/points")

LR = 0.01
EPOCH = 500
BATCH_SIZE = 4

labels_name = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10',
               '11', '12', '13', '14', '15', '16', '17', '18', '19']
color_list = ['r', 'k', 'y', 'g', 'c', 'm', 'coral', 'chocolate', 'saddlebrown', 'darkorange',
              'tan', 'gold', 'olive', 'greenyellow', 'cyan', 'dodgerblue', 'steelblue', 'blueviolet', 'plum', 'pink']

device = torch.device("cuda") if torch.cuda.is_available() else torch.device("cpu")

train_data = points_load.PointsDataset(txt=root + '/points-2-training.txt')
test_data = points_load.PointsDataset(txt=root + '/points-2-testing.txt')
train_loader = points_load.DataLoader(dataset=train_data, batch_size=BATCH_SIZE, shuffle=True)
test_loader = points_load.DataLoader(dataset=test_data, batch_size=BATCH_SIZE)

def plot_scatter(train_data, compared=None):
    train_body_data = train_data

    def plt_scatter(index, marker, color):
        plt.scatter(train_body_data[index][0][0], train_body_data[index][0][1], s=9, marker=marker, color=color)

    if compared:
        for i in range(len(train_body_data)):
            for j in range(20):
                if train_body_data[i][1] == j and compared[i][1] == j:
                    plt_scatter(i, marker='+', color=color_list[j])
                    break
                if j == 19:
                    plt_scatter(i, marker='v', color='black')
    else:
        for i in range(len(train_body_data)):
            for j in range(20):
                if train_body_data[i][1] == j:
                    plt_scatter(i, marker='+', color=color_list[j])
                    break
                if j == 19:
                    plt_scatter(i, marker='v', color='black')



def train():
    model = PlaneNet()
    model.to(device)
    print(model)
    print("Training in {device}".format(device=device))

    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LR, betas=(0.9, 0.99))
    scheduler = torch.optim.lr_scheduler.MultiStepLR(optimizer, milestones = [50, 150, 200], gamma=0.1)  # ???????????????????????????

    Acc = [[], []]
    Loss = [[], []]

    result_loss = 0
    result_acc = 0
    for epoch in range(EPOCH):
        start_time = time.time()

        print('#-------epoch {}--------'.format(epoch + 1))
        print('learning rate = ', optimizer.param_groups[0]['lr'])

        # ----------------------------????????????-------------------------------
        model.train()

        train_loss, train_acc = 0., 0.
        # ---------X,y = data, labels---------------
        for batch_x, batch_y in train_loader:
            batch_x = batch_x.to(device)
            batch_y = batch_y.to(device)
            batch_x, batch_y = Variable(batch_x), Variable(batch_y)
            out = model(batch_x)
            # --------------????????????????????????????????????????????????--------------
            loss = criterion(out, batch_y)
            optimizer.zero_grad()  # ????????????
            loss.backward()
            optimizer.step()  # ????????????
            # ------------????????????????????????----------------
            train_acc += (torch.max(out, 1)[1] == batch_y).sum().cpu().item()
            train_loss += loss.cpu().item()

        total_loss = (train_loss * float(BATCH_SIZE)) / len(train_data)
        total_acc = train_acc / (len(train_data))
        Acc[0].append(total_acc)
        Loss[0].append(total_loss)
        print('Train Loss: {:.6f}, Acc: {:.6f}'.format(total_loss, total_acc))

        # ----------------------------------????????????---------------------------------------
        model.eval()
        eval_loss = 0.
        eval_acc = 0.

        # ---------------------------------batch_x, batch_y  ??????  ??????----------------------------------
        y_true = []
        y_pred = []
        body_data = []
        pred_body_data = []
        # -------------------------------------------------------------------------------------------------
        with torch.no_grad():
            for batch_x, batch_y in test_loader:
                batch_x = batch_x.to(device)
                batch_y = batch_y.to(device)
                batch_x, batch_y = Variable(batch_x), Variable(batch_y)
                out = model(batch_x)
                # --------------????????????, ?????????----------
                loss = criterion(out, batch_y)
                eval_loss += loss.cpu().item()
                # --------------------?????????????????????????????????????????????????????????--------------
                y_true.extend(batch_y.cpu().numpy().tolist())
                y_pred.extend(torch.max(out, 1)[1].cpu().numpy().tolist())
                body_data.extend(batch_x.cpu().numpy().tolist())
                # -----------------------------------------------------------------------

                num_correct = (torch.max(out, 1)[1] == batch_y).sum()
                eval_acc += num_correct.cpu().item()

        # ------------------------------------??????????????????------------------------------
        if (epoch + 1) == EPOCH:

            for i in range(len(y_pred)):
                pred_body_data.append([body_data[i],y_pred[i]])
            print(pred_body_data)
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

        result_acc = total_acc
        result_loss = total_loss

        scheduler.step()  # ???????????????

    labels = ['train-acc', 'test-acc']
    colors = ['red', 'purple']
    styles = ["x", "+"]
    plt.subplot(121)
    k = 0
    for i, acc in enumerate(Acc):
        plt.plot(acc, label=labels[i], marker=styles[k], color=colors[k], linewidth=2)
        k = k + 1
    plt.title('acc----' + str(result_acc) + '---LR: ' + str(LR) + '----batchsize: ' + str(BATCH_SIZE))

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
    plt.title('loss----' + str(result_loss) + '---LR: ' + str(LR) + '----batchsize: ' + str(BATCH_SIZE))
    plt.legend(loc='best')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.show()

    plt.subplot(121)
    plt.title('LR: ' + str(LR))
    plot_scatter(train_data.get_data())
    plt.subplot(122)
    plot_scatter(pred_body_data, compared=test_data)
    plt.title('LR: ' + str(LR))
    plt.show()

    return

if __name__ == '__main__':
    train()