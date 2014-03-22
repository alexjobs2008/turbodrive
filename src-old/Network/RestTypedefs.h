#ifndef REST_TYPEDEFS_H
#define REST_TYPEDEFS_H

#include <QtCore/QSharedPointer>

struct RestResourceRequest;
typedef QSharedPointer<RestResourceRequest> RestResourceRequestRef;

struct RestResourceReply;
typedef QSharedPointer<RestResourceReply> RestResourceReplyRef;

class RestResource;
typedef QSharedPointer<RestResource> RestResourceRef;

class GenericRestResource;
typedef QSharedPointer<GenericRestResource> GenericRestResourceRef;

#endif // REST_TYPEDEFS_H