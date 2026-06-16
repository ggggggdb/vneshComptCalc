#ifndef BONUS_CALCULATOR_H
#define BONUS_CALCULATOR_H

#include <string>
#include <vector>

#include "AddInDefBase.h"

class CBonusCalculator : public IComponentBase {
   public:
    bool Init(void* pConnection) override;
    void Deinit() override;
    long GetInfo() override;
    bool RegisterExtensionAs(ExtensionKind extensionKind) override;

    long GetNMethods() const override;
    long FindMethod(const WCHAR_T* wsMethodName) const override;
    const WCHAR_T* GetMethodName(long lMethodNum, long lPropNum) const override;
    const WCHAR_T* GetMethodAlias(long lMethodNum, long lPropNum) const override;
    long GetNParams(long lMethodNum) const override;
    bool GetParamDefValue(long lMethodNum, long lParamNum, tVariant* pvarParamDef) const override;
    bool HasRetVal(long lMethodNum) const override;
    bool CallAsProc(long lMethodNum, tVariant* paParams, const long cbPaParams) override;
    bool CallAsFunc(long lMethodNum, tVariant* paParams, const long cbPaParams, tVariant* pvarRet) override;

   private:
    void* m_pConnection;
    double CalculateBonusImpl(const std::vector<double>& values);
};

#endif