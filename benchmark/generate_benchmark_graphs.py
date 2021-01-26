import os
import pandas as pd
import matplotlib.pyplot as plt
from pickle import dump, load
from typing import List
from re import sub as re_sub


def main() -> None:
    test_results_clang = benchmark_results(cc_compiler='clang-10',
                                           cxx_compiler='clang++-10',
                                           suite_name='Calling_every_class_from_the_hierarchy')
    test_results_gcc = benchmark_results(cc_compiler='gcc-10',
                                         cxx_compiler='g++-10',
                                         suite_name='Calling_every_class_from_the_hierarchy')
    with open('/tmp/res_clang', 'wb') as file:
        dump(test_results_clang, file)
    with open('/tmp/res_gcc', 'wb') as file:
        dump(test_results_gcc, file)

    # with open('/tmp/res_clang', 'rb') as file:
    #     test_results_clang = load(file)
    # with open('/tmp/res_gcc', 'rb') as file:
    #     test_results_gcc = load(file)

    show_graph(test_results_clang, test_results_gcc)


def benchmark_results(cc_compiler: str, cxx_compiler: str, suite_name: str) -> List:
    build_benchmark = execute(
        '('
        'rm -r build; '
        'mkdir build && cd build && '
        f'cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER={cc_compiler} '
        f'-DCMAKE_CXX_COMPILER={cxx_compiler} -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../ && '
        'make vstor-benchmark'
        ')')
    test_results_raw = execute('build/benchmark/vstor-benchmark --benchmark_repetitions=20')
    test_results = [
        *filter(lambda line: '_median' in line and suite_name in line,
                test_results_raw)]
    test_results = [*map(lambda line: re_sub('[ ]{2,}', ' ', line).split(' '), test_results)]
    test_results = [*map(lambda line: (line[0], float(line[1])), test_results)]
    return test_results


def execute(command: str) -> List[str]:
    print(command)
    return os.popen(command).readlines()


def show_graph(test_results_clang: List, test_results_gcc: List) -> None:
    test_results_clang = [*reversed(test_results_clang)]
    test_results_gcc = [*reversed(test_results_gcc)]

    test_names = [r[0].split('/')[1][0:-len('_median')] for r in test_results_clang]
    results_clang = [r[1] for r in test_results_clang]
    results_gcc = [r[1] for r in test_results_gcc]
    df = pd.DataFrame({'clang++-10': results_clang,
                       'g++-10': results_gcc}, index=test_names)
    ax = df.plot.barh()
    ax.set_xlabel('[ns]')

    size = len(results_clang)
    results_clang_relative = [x / results_clang[-1] for x in results_clang]
    results_gcc_relative = [x / results_gcc[-1] for x in results_gcc]
    for index, data_clang, data_gcc, data_clang_relative, data_gcc_relative in zip(
            range(size), results_clang, results_gcc, results_clang_relative,
            results_gcc_relative):
        fontsize = 10
        ax.text(y=index - 0.02 * fontsize, x=data_clang + 1,
                s=f"{data_clang} ({int(data_clang_relative * 100)}%)",
                fontdict=dict(fontsize=fontsize))
        ax.text(y=index + 0.005 * fontsize, x=data_gcc + 1,
                s=f"{data_gcc} ({int(data_gcc_relative * 100)}%)",
                fontdict=dict(fontsize=fontsize))
    plt.subplots_adjust(left=0.4, right=0.86)
    plt.savefig('benchmark/benchmark_chart.png')
    plt.show()


if __name__ == '__main__':
    main()
