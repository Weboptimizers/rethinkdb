// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_SERVERS_SERVER_STATUS_HPP_
#define CLUSTERING_ADMINISTRATION_SERVERS_SERVER_STATUS_HPP_

#include <string>
#include <vector>

#include "errors.hpp"
#include <boost/shared_ptr.hpp>

#include "clustering/administration/metadata.hpp"
#include "clustering/administration/servers/server_common.hpp"
#include "clustering/administration/servers/server_metadata.hpp"
#include "rdb_protocol/artificial_table/backend.hpp"
#include "rpc/semilattice/view.hpp"

class server_config_client_t;

class server_status_artificial_table_backend_t :
    public common_server_artificial_table_backend_t
{
public:
    server_status_artificial_table_backend_t(
            rdb_context_t *rdb_context,
            lifetime_t<name_resolver_t const &> name_resolver,
            watchable_map_t<peer_id_t, cluster_directory_metadata_t> *_directory,
            server_config_client_t *_server_config_client,
            admin_identifier_format_t _admin_format);
    ~server_status_artificial_table_backend_t();

    bool write_row(
            auth::user_context_t const &user_context,
            ql::datum_t primary_key,
            bool pkey_was_autogenerated,
            ql::datum_t *new_value_inout,
            signal_t *interruptor_on_caller,
            admin_err_t *error_out);

private:
    bool format_row(
            auth::user_context_t const &user_context,
            server_id_t const & server_id,
            peer_id_t const & peer_id,
            cluster_directory_metadata_t const & metadata,
            signal_t *interruptor_on_home,
            ql::datum_t *row_out,
            admin_err_t *error_out);

    std::map<peer_id_t, microtime_t> connect_times;

    server_config_client_t *server_config_client;
    admin_identifier_format_t admin_format;
    /* We use `directory_subs` to note when a server first connects. */
    watchable_map_t<peer_id_t, cluster_directory_metadata_t>::all_subs_t directory_subs;
};

#endif /* CLUSTERING_ADMINISTRATION_SERVERS_SERVER_STATUS_HPP_ */

