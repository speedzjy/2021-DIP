import torch
from torch.utils.data import Dataset, DataLoader
from PIL import Image
from torchvision import transforms

root = "../../data/insects"

class PointsDataset(Dataset):
    def __init__(self, txt):
        with open(txt, 'r') as fh:
            body_lengths = []
            for line in fh:
                line = line.strip('\n').rstrip()
                words = line.split()
                length = [float(words[0]), float(words[1])]
                body_lengths.append([length, int(words[2])])
            self.body_lengths = body_lengths

    def __getitem__(self, index):
        body_length, label = self.body_lengths[index]
        body_length = torch.Tensor(body_length)
        return body_length, label

    def __len__(self):
        return len(self.body_lengths)

    # def print_body(self, index):
    #     body_length, label = self.body_lengths[index]
    #     body_length = torch.Tensor(body_length)
    #     print(body_length)
    #
    def get_data(self):
        return self.body_lengths

if __name__ == '__main__':
    train_data = PointsDataset(txt=root + '/insects-training.txt')
    train_loader = DataLoader(dataset=train_data, batch_size=4)

    for x, y in train_loader:
        print(x)
        print(y)