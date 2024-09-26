#include <string>

#include <doctest/doctest.h>

#include "open62541pp/client.hpp"
#include "open62541pp/config.hpp"
#include "open62541pp/plugin/create_certificate.hpp"
#include "open62541pp/server.hpp"

#include "helper/server_runner.hpp"

using namespace opcua;

#ifdef UA_ENABLE_ENCRYPTION
#ifdef UAPP_CREATE_CERTIFICATE

TEST_CASE("Create certificate") {
    SUBCASE("Empty subject / subjectAltName") {
        CHECK_THROWS_AS(crypto::createCertificate({}, {}), CreateCertificateError);
    }

    SUBCASE("Invalid subject / subjectAltName") {
        CHECK_THROWS_AS(
            crypto::createCertificate({String{"X=Y"}}, {String{"DNS:localhost"}}),
            CreateCertificateError
        );
        CHECK_THROWS_AS(
            crypto::createCertificate({String{"C=DE"}}, {String{"X:Y"}}), CreateCertificateError
        );
    }

    SUBCASE("Valid") {
        const auto cert = crypto::createCertificate(
            {
                String{"C=DE"},
                String{"O=open62541pp"},
                String{"CN=open62541ppServer@localhost"},
            },
            {
                String{"DNS:localhost"},
                String{"URI:urn:open62541.server.application"},
            }
        );
        CHECK(!cert.privateKey.empty());
        CHECK(!cert.certificate.empty());
    }
}

TEST_CASE("Encrypted connection server/client") {
    const std::string serverApplicationUri = "urn:open62541.server.application";  // default
    const std::string clientApplicationUri = "urn:unconfigured:application";  // default

    // create server certificate
    const auto certServer = crypto::createCertificate(
        {
            String{"C=DE"},
            String{"O=open62541pp"},
            String{"CN=open62541ppServer@localhost"},
        },
        {
            String{"DNS:localhost"},
            String{std::string("URI:").append(serverApplicationUri)},
        }
    );

    // create client certificate
    const auto certClient = crypto::createCertificate(
        {
            String{"C=DE"},
            String{"O=open62541pp"},
            String{"CN=open62541ppClient@localhost"},
        },
        {
            String{"DNS:localhost"},
            String{std::string("URI:").append(clientApplicationUri)},
        }
    );

    SUBCASE("Connect without trusting each others certificate") {
        Server server(4840, certServer.certificate, certServer.privateKey, {}, {}, {});
        ServerRunner serverRunner(server);

        Client client(certClient.certificate, certClient.privateKey, {}, {});
        client.setSecurityMode(MessageSecurityMode::SignAndEncrypt);
#if UAPP_OPEN62541_VER_LE(1, 3)
        // TODO: doesn't throw with v1.4
        CHECK_THROWS(client.connect("opc.tcp://localhost:4840"));
#endif
    }

    SUBCASE("Connect with trust lists") {
        Server server(
            4840, certServer.certificate, certServer.privateKey, {certClient.certificate}, {}, {}
        );
        ServerRunner serverRunner(server);

        Client client(certClient.certificate, certClient.privateKey, {certServer.certificate}, {});
        client.setSecurityMode(MessageSecurityMode::SignAndEncrypt);
        CHECK_NOTHROW(client.connect("opc.tcp://localhost:4840"));
    }
}

#endif  // ifdef UAPP_CREATE_CERTIFICATE
#endif  // ifdef UA_ENABLE_ENCRYPTION