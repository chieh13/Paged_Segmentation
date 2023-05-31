import matplotlib.pyplot as plt

# 创建图形窗口
fig, ax = plt.subplots()

# 绘制方格图
def plot_array(array):
    # 清空图形
    ax.clear()

    # 计算行数和列数
    rows = len(array) // 10
    if len(array) % 10 != 0:
        rows += 1

    # 绘制方格和序号
    for i, value in enumerate(array):
        row = i // 10+1  # 行数
        col = i % 10   # 列数

        if value == 0:
            color = 'green'
        else:
            color = 'red'

        # 绘制方格
        rect = plt.Rectangle((col + 0.1, -row - 0.1), 0.8, 0.8, color=color, linewidth=1, edgecolor='black')
        ax.add_patch(rect)

        # 添加序号
        ax.annotate(str(i), (col + 0.5, -row + 0.5), color='white', weight='bold', ha='center', va='center')

    # 设置坐标轴范围
    ax.set_xlim(-0.5, 10.5)
    ax.set_ylim(-rows - 0.5, 0.5)

    # 隐藏坐标轴
    ax.axis('off')

    # 刷新图形
    plt.draw()
    plt.pause(0.001)

# 循环读取数组并更新图形
while True:
    with open('D:\\Code\\OS\\test\\memory.txt', 'r') as file:
        content = file.read()

    array = [int(x) for x in content.strip().split() if x.strip() != '']

    plot_array(array)
