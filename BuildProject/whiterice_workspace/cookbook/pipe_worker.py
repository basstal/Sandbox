import os
import whiterice


class PipeWorker(whiterice.ITable):
    @whiterice.priority(-10)
    def make(self, cook: whiterice.Cook) -> int:
        # 通过 dotnet 编译运行 SandboxPipeWorker 项目
        sandbox_pipe_worker_dir = os.path.join(
            cook.config.project, "Programs/SandboxPipeWorker"
        )
        cook.exec.execute_straight("dotnet", ["run"], work_dir=sandbox_pipe_worker_dir)
        return super().make(cook)
