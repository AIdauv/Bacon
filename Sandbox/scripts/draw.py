import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import numpy as np

# 设置学术论文级别的绘图参数
plt.rcParams.update({
    'font.size': 12,
    'axes.titlesize': 14,
    'axes.labelsize': 12,
    'xtick.labelsize': 10,
    'ytick.labelsize': 10,
    'legend.fontsize': 10,
    'figure.dpi': 300,
    'figure.figsize': (8, 6),
    'savefig.dpi': 300,
    'savefig.format': 'tiff',  # 改为TIFF格式
    'savefig.bbox': 'tight',
    'font.family': 'serif',
    'font.serif': ['Times New Roman'],
    'mathtext.fontset': 'stix',
    'axes.grid': True,
    'grid.linestyle': '--',
    'grid.alpha': 0.3
})

# 文件路径处理
file_path = 'perf_test_results.csv'

# 检查文件是否存在
if not os.path.exists(file_path):
    print(f"错误：文件不存在 - {file_path}")
    exit()

# 读取CSV数据
try:
    df = pd.read_csv(file_path)
    print("直接使用原始数据，不进行单位转换")
    
    # 验证GridSize=1时的值
    grid1 = df[df['GridSize'] == 1].iloc[0]
    print("\nGridSize=1验证值:")
    print(f"BatchGPUTime: {grid1['BatchGPUTime']:.6f} ms")
    print(f"TraditionalGPUTime: {grid1['TraditionalGPUTime']:.6f} ms")
    
except Exception as e:
    print(f"读取CSV文件时出错: {e}")
    exit()

# 创建输出目录
output_dir = "performance_plots"
os.makedirs(output_dir, exist_ok=True)

# 定义颜色方案
batch_color = '#1f77b4'  # 蓝色
traditional_color = '#ff7f0e'  # 橙色
ratio_color = '#2ca02c'  # 绿色

# 1. 总时间对比图
plt.figure()
plt.plot(df['GridSize'], df['BatchTotalTime'], 'o-', color=batch_color, 
         markersize=6, linewidth=2, label='Batch Total Time')
plt.plot(df['GridSize'], df['TraditionalTotalTime'], 's-', color=traditional_color, 
         markersize=5, linewidth=2, label='Traditional Total Time')
plt.xlabel('Grid Size')
plt.ylabel('Time (ms)')
plt.title('Total Time Comparison')
plt.legend()
plt.yscale('log')
plt.grid(True, linestyle='--', alpha=0.5)

# 添加GridSize=1标注
grid1_idx = df[df['GridSize'] == 1].index[0]
plt.annotate(f"Batch: {df.loc[grid1_idx, 'BatchTotalTime']:.6f} ms\nTrad: {df.loc[grid1_idx, 'TraditionalTotalTime']:.6f} ms",
             xy=(1, df.loc[grid1_idx, 'TraditionalTotalTime']),
             xytext=(20, 30),
             textcoords='offset points',
             arrowprops=dict(arrowstyle='->'),
             bbox=dict(boxstyle='round,pad=0.3', fc='yellow', alpha=0.2))

plt.savefig(os.path.join(output_dir, 'total_time_comparison.tiff'))  # 改为TIFF
plt.close()

# 2. BatchAddTime与TraditionalGenTime对比
plt.figure()
plt.plot(df['GridSize'], df['BatchAddTime'], 'o-', color=batch_color, 
         markersize=6, linewidth=2, label='Batch Add Time')
plt.plot(df['GridSize'], df['TraditionalGenTime'], 's-', color=traditional_color, 
         markersize=5, linewidth=2, label='Traditional Generation Time')
plt.xlabel('Grid Size')
plt.ylabel('Time (ms)')
plt.title('Data Generation Time Comparison')
plt.legend()
plt.yscale('log')
plt.grid(True, linestyle='--', alpha=0.5)
plt.savefig(os.path.join(output_dir, 'generation_time_comparison.tiff'))  # 改为TIFF
plt.close()

# 3. BatchFlushTime与TraditionalUploadTime对比
plt.figure()
plt.plot(df['GridSize'], df['BatchFlushTime'], 'o-', color=batch_color, 
         markersize=6, linewidth=2, label='Batch Flush Time')
plt.plot(df['GridSize'], df['TraditionalUploadTime'], 's-', color=traditional_color, 
         markersize=5, linewidth=2, label='Traditional Upload Time')
plt.xlabel('Grid Size')
plt.ylabel('Time (ms)')
plt.title('Data Upload Time Comparison')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.5)
plt.savefig(os.path.join(output_dir, 'upload_time_comparison.tiff'))  # 改为TIFF
plt.close()

# 4. BatchGPUTime与TraditionalGPUTime对比 - 直接使用原始数据
plt.figure()
plt.plot(df['GridSize'], df['BatchGPUTime'], 'o-', color=batch_color, 
         markersize=6, linewidth=2, label='Batch GPU Time')
plt.plot(df['GridSize'], df['TraditionalGPUTime'], 's-', color=traditional_color, 
         markersize=5, linewidth=2, label='Traditional GPU Time')
plt.xlabel('Grid Size')
plt.ylabel('Time (ms)')
plt.title('GPU Processing Time Comparison')
plt.legend()

# 添加GridSize=1标注
plt.annotate(f"Batch: {grid1['BatchGPUTime']:.6f} ms\nTrad: {grid1['TraditionalGPUTime']:.6f} ms",
             xy=(1, grid1['TraditionalGPUTime']),
             xytext=(20, 20),
             textcoords='offset points',
             arrowprops=dict(arrowstyle='->'),
             bbox=dict(boxstyle='round,pad=0.3', fc='yellow', alpha=0.2))

# 添加说明：为什么BatchGPU在GridSize=1时较高
plt.text(100, 0.001, "Small Grids: Batch overhead > benefit\nLarge Grids: Batch significantly faster",
         bbox=dict(boxstyle='round,pad=0.5', fc='lightyellow', ec='gold', alpha=0.7))

plt.yscale('log')
plt.grid(True, linestyle='--', alpha=0.5)
plt.savefig(os.path.join(output_dir, 'gpu_time_comparison.tiff'))  # 改为TIFF
plt.close()

# 5. Draw Calls对比
plt.figure()
plt.plot(df['GridSize'], df['BatchDrawCalls'], 'o-', color=batch_color, 
         markersize=6, linewidth=2, label='Batch Draw Calls')
plt.plot(df['GridSize'], df['TraditionalDrawCalls'], 's-', color=traditional_color, 
         markersize=5, linewidth=2, label='Traditional Draw Calls')
plt.xlabel('Grid Size')
plt.ylabel('Number of Draw Calls')
plt.title('Draw Calls Comparison')
plt.legend()
plt.yscale('log')
plt.grid(True, linestyle='--', alpha=0.5)

# 添加二次函数参考线
x = np.linspace(df['GridSize'].min(), df['GridSize'].max(), 100)
y = x**2  # 二次函数参考线
plt.plot(x, y, 'k--', alpha=0.5, linewidth=1.5, label='$n^2$ Reference')
plt.legend()

# 添加说明文本
plt.text(300, 1000, "Traditional method follows\nquadratic complexity ($O(n^2)$)", 
         fontsize=10, rotation=0, 
         bbox=dict(boxstyle="round,pad=0.3", fc="lavender", ec="purple", alpha=0.7))

plt.savefig(os.path.join(output_dir, 'draw_calls_comparison.tiff'))  # 改为TIFF
plt.close()

# 6. 加速比 (Ratio) - 优化版本
plt.figure(figsize=(8, 6))

# 主数据线
plt.plot(df['GridSize'], df['Ratio'], 'd-', color=ratio_color, markersize=6, 
         linewidth=2, label='Speedup Ratio')

# 参考线
plt.axhline(y=1, color='r', linestyle='--', linewidth=1.5, label='Baseline (1x)')

# 添加最大和最小加速比标注
max_ratio = df['Ratio'].max()
max_idx = df['Ratio'].idxmax()
min_ratio = df['Ratio'].min()
min_idx = df['Ratio'].idxmin()

# 最大加速比标注（置于数据点下方）
plt.text(df['GridSize'][max_idx], max_ratio * 0.85,  # 放置在点下方
         f'Max: {max_ratio:.1f}x\n(Grid={df["GridSize"][max_idx]})',
         horizontalalignment='center',
         verticalalignment='top',
         fontsize=10,
         bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.8))

# 最小加速比标注（置于数据点上方）
plt.text(df['GridSize'][min_idx], min_ratio * 1.15,  # 放置在点上方
         f'Min: {min_ratio:.1f}x\n(Grid={df["GridSize"][min_idx]})',
         horizontalalignment='center',
         verticalalignment='bottom',
         fontsize=10,
         bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="gray", alpha=0.8))

# 添加平均加速比线
avg_ratio = df['Ratio'].mean()
plt.axhline(y=avg_ratio, color='g', linestyle='-', alpha=0.7, linewidth=1.5, 
            label=f'Avg: {avg_ratio:.1f}x')

# 添加趋势线（多项式拟合）
z = np.polyfit(df['GridSize'], df['Ratio'], 1)
p = np.poly1d(z)
plt.plot(df['GridSize'], p(df['GridSize']), "k--", alpha=0.5, linewidth=1.5, 
         label='Linear Trend')

# 标签和标题
plt.xlabel('Grid Size')
plt.ylabel('Speedup Ratio (Traditional / Batch)')
plt.title('Performance Speedup Ratio', pad=20)

# 图例位置调整到右下角
plt.legend(loc='lower right', frameon=True, framealpha=0.8)

# 设置坐标轴范围
plt.ylim(0, max_ratio * 1.2)
plt.xlim(0, df['GridSize'].max() * 1.05)

plt.grid(True, linestyle='--', alpha=0.5)
plt.savefig(os.path.join(output_dir, 'speedup_ratio.tiff'))  # 改为TIFF
plt.close()

# 创建关键数据点表格
def create_summary_table():
    key_points = df[(df['GridSize'] == 1) | 
                   (df['GridSize'] == 100) | 
                   (df['GridSize'] == 256) | 
                   (df['GridSize'] == 496)]
    
    # 选择要显示的列
    summary = key_points[['GridSize', 
                         'BatchTotalTime', 'TraditionalTotalTime', 'Ratio',
                         'BatchGPUTime', 'TraditionalGPUTime',
                         'BatchDrawCalls', 'TraditionalDrawCalls']].copy()
    
    # 重命名列
    summary.columns = ['Grid Size', 
                      'Batch Total (ms)', 'Trad Total (ms)', 'Speedup Ratio',
                      'Batch GPU (ms)', 'Trad GPU (ms)',
                      'Batch Draws', 'Trad Draws']
    
    # 格式化数字
    summary['Speedup Ratio'] = summary['Speedup Ratio'].map('{:.1f}x'.format)
    for col in ['Batch Total (ms)', 'Trad Total (ms)', 'Batch GPU (ms)', 'Trad GPU (ms)']:
        summary[col] = summary[col].map('{:.6f}'.format)
    
    # 创建表格图
    plt.figure(figsize=(10, 3))
    ax = plt.subplot(111, frame_on=False)
    ax.xaxis.set_visible(False)
    ax.yaxis.set_visible(False)
    
    # 创建表格
    table = plt.table(cellText=summary.values,
                     colLabels=summary.columns,
                     cellLoc='center',
                     loc='center',
                     bbox=[0, 0, 1, 1])
    
    # 设置表格样式
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.5)
    
    # 标题
    plt.title('Key Performance Metrics at Selected Grid Sizes', pad=20)
    
    plt.savefig(os.path.join(output_dir, 'performance_summary_table.tiff'))  # 改为TIFF
    plt.close()

# 生成表格
create_summary_table()

print(f"所有图表已保存至目录: {output_dir}")
print(f"包括6个性能对比图和1个摘要表格，格式为TIFF")