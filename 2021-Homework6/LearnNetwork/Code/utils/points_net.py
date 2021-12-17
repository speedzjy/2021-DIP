import torch
import torch.nn as nn
import torch.nn.functional as F


class InsectNet(nn.Module):
    def __init__(self):
        super(InsectNet, self).__init__()

        self.fc1 = nn.Sequential(
            nn.Linear(2, 200),
            nn.Sigmoid()
        )

        self.fc2 = nn.Sequential(
            nn.Linear(200, 50),
            nn.Sigmoid()
        )

        self.fc3 = nn.Sequential(
            nn.Linear(50, 3),
        )

    def forward(self, x):
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x


class PlaneNet(nn.Module):
    def __init__(self):
        super(PlaneNet, self).__init__()

        self.fc1 = nn.Sequential(
            nn.Linear(2, 500),
            nn.ReLU()
        )

        self.fc2 = nn.Sequential(
            nn.Linear(500, 100),
            nn.ReLU()
        )

        self.fc3 = nn.Sequential(
            nn.Linear(100, 20),
        )

    def forward(self, x):
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)
        return x
