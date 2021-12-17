import torch.nn as nn
import torch.nn.functional as F


class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()

        #  原始输入 28*28*1
        self.conv1 = nn.Sequential(
            # padding后变成32*32*1
            nn.Conv2d(1, 16, 5, 1, 2),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2)
        )
        # conv1输出为(16, 14, 14)    14*14*16   H*W*channels
        self.conv2 = nn.Sequential(
            # padding后变成18*18*32
            nn.Conv2d(16, 32, 5, 1, 2),
            nn.ReLU(),
            nn.MaxPool2d(2)
        )
        # conv2输出为(32, 7, 7)  7*7*32
        self.fc1 = nn.Linear(32 * 7 * 7, 1200)
        self.fc2 = nn.Linear(1200, 640)
        self.fc3 = nn.Linear(640, 100)
        self.fc4 = nn.Linear(100, 10)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = self.fc4(x)
        return x
