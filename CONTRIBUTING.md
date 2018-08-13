# Contributing

If you discover issues, have ideas for improvements or new features, or
want to contribute a new module, please report them to the
[issue tracker][1] of the repository or submit a pull request. Please,
try to follow these guidelines when you do so.

## Issue reporting

* Check that the issue has not already been reported.
* Check that the issue has not already been fixed in the latest code
  (a.k.a. `master`).
* Be clear, concise and precise in your description of the problem.
* Open an issue with a descriptive title and a summary in grammatically correct,
  complete sentences.
* Include any relevant code to the issue summary.

## Pull requests

* Use a topic branch to easily amend a pull request later, if necessary.
* Write good commit messages.
* Use the same coding conventions as the rest of the project.
* Ensure your edited codes with four spaces instead of TAB.
* Please commit code to `dev` branch and we will merge into `master` branch in feature
* Before adding new features and new modules, please go to issues to submit the relevant feature description first

# 贡献代码

如果你发现一些问题，或者想新增或者改进某些新特性，或者想贡献一个新的模块
那么你可以在[issues][1]上提交反馈，或者发起一个提交代码的请求(pull request).

## 问题反馈

* 确认这个问题没有被反馈过
* 确认这个问题最近还没有被修复，请先检查下 `master` 的最新提交
* 请清晰详细地描述你的问题
* 如果发现某些代码存在问题，请在issue上引用相关代码

## 提交代码

* 请先更新你的本地分支到最新，再进行提交代码请求，确保没有合并冲突
* 编写友好可读的提交信息
* 请使用与工程代码相同的代码规范
* 请提交代码到`dev`分支，如果通过，我们会在特定时间合并到`master`分支上
* 为了规范化提交日志的格式，commit消息，不要用中文，请用英文描述
* 增加新特性和新模块之前，请先到issues提交相关特性说明，经过讨论评估确认后，再进行相应的代码提交，避免做无用工作

## 编码规范

代码需要符合tbox的编码风格，保证整体风格一致，这样可读性会更好，也更容易维护，下面列举一些风格描述。

* 空格缩进、填充，4字符宽度，不允许出现任意tab字符
* 换行符'\n'，不要用'\r\n'
* 文件utf8编码，不允许其他编码格式，例如：gbk
* 全英文注释，public接口注释采用doxygen风格
* 类unix命名规范：小写 + 下划线，例如：`aaa_bb_cc`，不允许任何大写字符
* 接口名、宏定义等必须带：`tb_`, `TB_`前缀
* 需要使用tbox提供的基础类型，例如：`tb_size_t`
* if, while, for等关键字之后，带一个空格，例如：`if ()`, `for (; ;)`
* 如果if, for的body只有一行代码，不需要写大括号
* {}块，换行对称
* 局部变量定义不需要遵循c89风格，定义位置尽量靠近实际使用的地方
* 函数取参，返回值处理，需要有assert检测
* 宏定义大写，其他都小写
* 除非必要，尽量使用`tb_size_t`, `tb_long_t`类型，而不是`tb_int_t`
* 接口定义、接口实现、宏定义、全局变量、include等布局风格必须一致，风格可参考实际代码


[1]: https://github.com/tboox/tbox/issues
