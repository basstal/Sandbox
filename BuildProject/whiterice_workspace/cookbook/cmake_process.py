# import os
# import shutil
# import whiterice


# class CMakeProcess(whiterice.ITable):
#     # TODO:检查 cmake 是否在环境中
#     @whiterice.priority(10)
#     def make(self, cook: whiterice.Cook) -> int:
#         self.env_copy = os.environ.copy()
#         self.env_copy["VSLANG"] = "1033" # 使 msvc 构建输出设置为英文，避免乱码问题
#         self.build_dir = os.path.join(cook.config.project, "build")
#         if not os.path.isdir(self.build_dir):
#             if os.path.exists(self.build_dir):
#                 shutil.rmtree(self.build_dir)
#             os.makedirs(self.build_dir)
#         cook.exec.execute_straight(
#             "cmake", [".."], work_dir=self.build_dir, env=self.env_copy
#         )
#         return super().make(cook)

#     def build(self, cook: whiterice.Cook) -> int:
#         self.env_copy["UseMultiToolTask"] = (
#             "true"  # 参考 https://zhuanlan.zhihu.com/p/667591876 和 https://devblogs.microsoft.com/cppblog/improved-parallelism-in-msbuild/
#         )
#         cook.exec.execute_straight(
#             "cmake", ["--build", "."], work_dir=self.build_dir, env=self.env_copy
#         )
#         return super().build(cook)
