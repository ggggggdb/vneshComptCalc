#include "BonusCalculator.h"

#include <algorithm>
#include <cstring>

static const wchar_t* g_MethodNames[] = {L"CalculateBonus", nullptr};
static const wchar_t* g_MethodNamesRu[] = {L"РассчитатьПремию", nullptr};

bool CBonusCalculator::Init(void* pConnection) {
    m_pConnection = pConnection;
    return true;
}
void CBonusCalculator::Deinit() {}
long CBonusCalculator::GetInfo() {
    return 0;
}
bool CBonusCalculator::RegisterExtensionAs(ExtensionKind extensionKind) {
    return true;
}
long CBonusCalculator::GetNMethods() const {
    return 1;
}

long CBonusCalculator::FindMethod(const WCHAR_T* wsMethodName) const {
    if (!wsMethodName)
        return -1;
    for (int i = 0; i < 1; i++) {
        if (g_MethodNames[i] && std::wcscmp(wsMethodName, g_MethodNames[i]) == 0)
            return i;
        if (g_MethodNamesRu[i] && std::wcscmp(wsMethodName, g_MethodNamesRu[i]) == 0)
            return i;
    }
    return -1;
}

const WCHAR_T* CBonusCalculator::GetMethodName(long lMethodNum, long lPropNum) const {
    if (lMethodNum == 0 && lPropNum == 0)
        return g_MethodNames[0];
    return nullptr;
}
const WCHAR_T* CBonusCalculator::GetMethodAlias(long lMethodNum, long lPropNum) const {
    if (lMethodNum == 0 && lPropNum == 0)
        return g_MethodNamesRu[0];
    return nullptr;
}
long CBonusCalculator::GetNParams(long lMethodNum) const {
    return (lMethodNum == 0) ? 1 : 0;
}
bool CBonusCalculator::GetParamDefValue(long, long, tVariant*) const {
    return false;
}
bool CBonusCalculator::HasRetVal(long lMethodNum) const {
    return (lMethodNum == 0);
}
bool CBonusCalculator::CallAsProc(long, tVariant*, const long) {
    return false;
}

double CBonusCalculator::CalculateBonusImpl(const std::vector<double>& v) {
    double hoursActual = v[0];
    double hoursPlan = v[1];
    double grades[5] = {v[2], v[3], v[4], v[5], v[6]};
    double weights[5] = {v[7], v[8], v[9], v[10], v[11]};
    double baseRate = v[12];
    if (hoursPlan <= 0)
        return 0.0;
    double hourCoeff = hoursActual / hoursPlan;
    if (hourCoeff > 1.5)
        hourCoeff = 1.5;
    double wsum = 0.0, wsumWeights = 0.0;
    for (int i = 0; i < 5; ++i) {
        wsum += grades[i] * weights[i];
        wsumWeights += weights[i];
    }
    double avgGrade = (wsumWeights > 0) ? (wsum / wsumWeights) : 0.0;
    return hoursActual * hourCoeff * avgGrade * baseRate;
}

bool CBonusCalculator::CallAsFunc(long lMethodNum, tVariant* paParams, const long cbPaParams, tVariant* pvarRet) {
    if (lMethodNum != 0)
        return false;
    if (!paParams || cbPaParams < 1)
        return false;
    if (paParams[0].vt != VTYPE_PWSTR)
        return false;
    const wchar_t* ws = paParams[0].pwstrVal;
    if (!ws)
        return false;
    std::vector<double> values;
    std::wstring input(ws);
    size_t start = 0;
    while (start < input.length()) {
        size_t end = input.find(L';', start);
        if (end == std::wstring::npos)
            end = input.length();
        std::wstring token = input.substr(start, end - start);
        if (!token.empty())
            values.push_back(std::stod(token));
        start = end + 1;
    }
    if (values.size() < 13)
        return false;
    double bonus = CalculateBonusImpl(values);
    pvarRet->vt = VTYPE_R8;
    pvarRet->dblVal = bonus;
    return true;
}

extern "C" __declspec(dllexport) bool GetClassObject(const WCHAR_T* wsClassName, IComponentBase** ppComponent) {
    if (!wsClassName || !ppComponent)
        return false;
    if (std::wcscmp(wsClassName, L"BonusCalculator") == 0) {
        *ppComponent = new CBonusCalculator();
        return true;
    }
    return false;
}

extern "C" __declspec(dllexport) bool DestroyObject(IComponentBase** ppComponent) {
    if (!ppComponent || !*ppComponent)
        return false;
    delete *ppComponent;
    *ppComponent = nullptr;
    return true;
}

extern "C" __declspec(dllexport) bool GetClassNames(WCHAR_T*** pwsClassNames, long* plSize) {
    static WCHAR_T* classNames[] = {L"BonusCalculator", nullptr};
    *pwsClassNames = classNames;
    *plSize = 1;
    return true;
}