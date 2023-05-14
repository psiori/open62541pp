#include "open62541pp/types/Composed.h"

namespace opcua {

BrowseDescription::BrowseDescription(
    const NodeId& nodeId,
    BrowseDirection browseDirection,
    const NodeId& referenceType,
    bool includeSubtypes,
    uint32_t nodeClassMask,
    uint32_t resultMask
) {
    asWrapper<NodeId>(handle()->nodeId) = nodeId;
    handle()->browseDirection = static_cast<UA_BrowseDirection>(browseDirection);
    asWrapper<NodeId>(handle()->referenceTypeId) = referenceType;
    handle()->includeSubtypes = includeSubtypes;
    handle()->nodeClassMask = nodeClassMask;
    handle()->resultMask = resultMask;
}

RelativePathElement::RelativePathElement(
    const NodeId& referenceType,
    bool isInverse,
    bool includeSubtypes,
    const QualifiedName& targetName
) {
    asWrapper<NodeId>(handle()->referenceTypeId) = referenceType;
    handle()->isInverse = isInverse;
    handle()->includeSubtypes = includeSubtypes;
    asWrapper<QualifiedName>(handle()->targetName) = targetName;
}

RelativePath::RelativePath(std::initializer_list<RelativePathElement> elements) {
    handle()->elementsSize = elements.size();
    handle()->elements = detail::toNativeArrayAlloc(elements.begin(), elements.end());
}

RelativePath::RelativePath(const std::vector<RelativePathElement>& elements) {
    handle()->elementsSize = elements.size();
    handle()->elements = detail::toNativeArrayAlloc(elements.begin(), elements.end());
}

BrowsePath::BrowsePath(const NodeId& startingNode, const RelativePath& relativePath) {
    asWrapper<NodeId>(handle()->startingNode) = startingNode;
    asWrapper<RelativePath>(handle()->relativePath) = relativePath;
}

ReadValueId::ReadValueId(const NodeId& id, AttributeId attributeId) {
    asWrapper<NodeId>(handle()->nodeId) = id;
    handle()->attributeId = static_cast<uint32_t>(attributeId);
}

}  // namespace opcua
