import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np

# 데이터 생성
np.random.seed(1)
x_train = np.random.randint(0, 100, size=(6, 4))
y_train = np.full((6,), 7.5)

# min-max 정규화 함수
def min_max_normalize(data):
    numerator = data - np.min(data, 0)
    denominator = np.max(data, 0) - np.min(data, 0)
    return numerator / (denominator + 1e-7)

# 데이터 정규화
x_train_norm = min_max_normalize(x_train)
y_train_norm = min_max_normalize(y_train.reshape(-1, 1))

# 파이토치 모델 정의
class Net(nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.fc1 = nn.Linear(4, 16)
        self.fc2 = nn.Linear(16, 8)
        self.fc3 = nn.Linear(8, 1)

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.fc3(x)
        return x

# 모델 생성
net = Net()

# 손실 함수와 최적화 함수 정의
criterion = nn.MSELoss()
optimizer = optim.SGD(net.parameters(), lr=0.01)

# 학습 시작
for epoch in range(10000):
    inputs = torch.tensor(x_train_norm, dtype=torch.float32)
    labels = torch.tensor(y_train_norm, dtype=torch.float32)

    optimizer.zero_grad()
    outputs = net(inputs)
    loss = criterion(outputs, torch.tensor(y_train_norm, dtype=torch.float32) * 7.5)
    loss.backward()
    optimizer.step()

    if epoch % 1000 == 0:
        print(f"epoch {epoch}, loss {loss.item():.4f}")

# 예측 시작
x_test = np.random.randint(0, 100, size=(1, 4))
x_test_norm = min_max_normalize(x_test)
y_pred_norm = net(torch.tensor(x_test_norm, dtype=torch.float32)).detach().numpy()
y_pred = y_pred_norm * (np.max(y_train) - np.min(y_train)) + np.min(y_train)

y_true_norm = min_max_normalize(np.array([7.5]).reshape(-1, 1))
y_true = y_true_norm * (np.max(y_train) - np.min(y_train)) + np.min(y_train)
accuracy = (1 - np.abs((y_true - y_pred_norm))) * 100

print(f"input: {x_test}, predicted output: {float(y_pred[0]):.4f}, expected output: {float(y_true[0]):.4f}, accuracy: {float(accuracy[0]):.2f}%")


