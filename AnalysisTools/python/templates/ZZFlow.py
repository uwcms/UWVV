from UWVV.AnalysisTools.templates.ZZID import ZZID
from UWVV.AnalysisTools.templates.ZZIso import ZZIso
from UWVV.AnalysisTools.templates.ZZFSR import ZZFSR



class ZZFlow(ZZIso, ZZFSR, ZZID):
    def __init__(self, *args, **kwargs):
        super(ZZFlow, self).__init__(*args, **kwargs)


    def makeAnalysisStep(self, stepName, **inputs):
        return super(ZZFlow, self).makeAnalysisStep(stepName, **inputs)
