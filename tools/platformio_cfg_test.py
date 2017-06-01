from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

env.Append(
        LINKFLAGS=[
            "--coverage"
            ]
        )
