/* Real UDP sockets and production ICE lifecycle; deliberately allocate fd 0.
 * A reconnect must release it, and repeated destruction must be harmless. */
#include "agent.c"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

/* RNG/crypto lifecycle is exercised with real mbedTLS in ice_transport.
 * Keep this fd-0/socket ownership test independent of crypto libraries. */
UtilsRandom* utils_random_create(void) { return (UtilsRandom*)1; }
void utils_random_destroy(UtilsRandom* random) { (void)random; }

int main(void)
{
    int saved_stdin = dup(STDIN_FILENO);
    close(STDIN_FILENO);
    for (unsigned attempt = 0; attempt < 8; attempt++) {
        Agent agent = {0};
        assert(agent_create(&agent) == 0);
        assert(agent.udp_sockets[0].fd == 0);
        assert(agent.udp_sockets[1].fd == -1);
        int sender = socket(AF_INET, SOCK_DGRAM, 0);
        assert(sender > 0);
        struct sockaddr_in destination = agent.udp_sockets[0].bind_addr.sin;
        destination.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        assert(sendto(sender, "hello", 5, 0, (struct sockaddr *)&destination, sizeof(destination)) == 5);
        uint8_t packet[16];
        Address source = {0};
        /* The production poll is only 1 ms; virtual host loopback delivery can
         * take longer under a parallel firmware build. Keep the fd 0 check,
         * but allow bounded retries using the production receive helper. */
        assert(agent_socket_recv_attempts(&agent, &source, packet, sizeof(packet), 1000) == 5);
        assert(!memcmp(packet, "hello", 5));
        close(sender);
        agent_destroy(&agent);
        assert(agent.udp_sockets[0].fd == -1);
        errno = 0;
        assert(fcntl(0, F_GETFD) == -1 && errno == EBADF);
        assert(agent_socket_recv(&agent, &source, packet, sizeof(packet), 1) == 0);
        int replacement = dup(STDOUT_FILENO);
        assert(replacement == 0);
        agent_destroy(&agent);
        assert(fcntl(replacement, F_GETFD) >= 0);
        close(replacement);
    }
    if (saved_stdin >= 0) {
        assert(dup2(saved_stdin, STDIN_FILENO) == STDIN_FILENO);
        close(saved_stdin);
    }
    puts("ICE fd 0 receive/close and repeated reconnect lifecycle passed");
    return 0;
}
