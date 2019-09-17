__version__ = "3.0"

from meshroom.core import desc


class ULR(desc.CommandLineNode):
    commandLine = 'SIBR_ulrv2_app_rwdi {allParams}'

    cpu = desc.Level.INTENSIVE
    ram = desc.Level.INTENSIVE

    inputs = [
        desc.File(
            name='path',
            label='Input Folder',
            description='MeshroomCache folder containing the StructureFromMotion folder, PrepareDenseScene folder, and Texturing folder.',
            value=desc.Node.internalFolder + '/../../',
            uid=[0],
        ),
        desc.ChoiceParam(
            name='texture-width',
            label='Texture Width',
            description='''Output texture size''',
            value=1024,
            values=(256, 512, 1024, 2048, 4096),
            exclusive=True,
            uid=[0],
        ),
    ]

    outputs = [
    ]
