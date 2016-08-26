// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef CLUSTERING_ADMINISTRATION_TABLES_TABLE_CONFIG_HPP_
#define CLUSTERING_ADMINISTRATION_TABLES_TABLE_CONFIG_HPP_

#include <string>
#include <vector>

#include "errors.hpp"
#include <boost/shared_ptr.hpp>

#include "clustering/administration/servers/config_client.hpp"
#include "clustering/administration/tables/table_common.hpp"
#include "containers/uuid.hpp"
#include "rdb_protocol/artificial_table/backend.hpp"
#include "rpc/semilattice/view.hpp"

class real_reql_cluster_interface_t;

/* This is publicly exposed so that it can be used to create the return value of
`table.reconfigure()`. */
ql::datum_t convert_table_config_to_datum(
        namespace_id_t table_id,
        const ql::datum_t &db_name_or_uuid,
        const table_config_t &config,
        admin_identifier_format_t identifier_format,
        const server_name_map_t &server_names);

class table_config_artificial_table_backend_t :
    public common_table_artificial_table_backend_t
{
public:
    table_config_artificial_table_backend_t(
            rdb_context_t *rdb_context,
            lifetime_t<name_resolver_t const &> name_resolver,
            boost::shared_ptr<semilattice_readwrite_view_t<
                cluster_semilattice_metadata_t> > _semilattice_view,
            real_reql_cluster_interface_t *_reql_cluster_interface,
            admin_identifier_format_t _identifier_format,
            server_config_client_t *_server_config_client,
            table_meta_client_t *_table_meta_client);
    ~table_config_artificial_table_backend_t();

    bool write_row(
            auth::user_context_t const &user_context,
            ql::datum_t primary_key,
            bool pkey_was_autogenerated,
            ql::datum_t *new_value_inout,
            signal_t *interruptor_on_caller,
            admin_err_t *error_out);

private:
    void format_row(
            auth::user_context_t const &user_context,
            const namespace_id_t &table_id,
            const table_config_and_shards_t &config,
            const ql::datum_t &db_name_or_uuid,
            signal_t *interruptor_on_home,
            ql::datum_t *row_out)
            THROWS_ONLY(interrupted_exc_t, no_such_table_exc_t, failed_table_op_exc_t);

    void do_modify(
        const namespace_id_t &table_id,
        table_config_and_shards_t &&old_config,
        table_config_t &&new_config_no_shards,
        server_name_map_t &&new_server_names,
        const name_string_t &old_db_name,
        const name_string_t &new_db_name,
        signal_t *interruptor)
        THROWS_ONLY(interrupted_exc_t, no_such_table_exc_t, failed_table_op_exc_t,
            maybe_failed_table_op_exc_t, admin_op_exc_t);

    void do_create(
        const namespace_id_t &table_id,
        table_config_t &&new_config_no_shards,
        server_name_map_t &&new_server_names,
        const name_string_t &new_db_name,
        signal_t *interruptor)
        THROWS_ONLY(interrupted_exc_t, no_such_table_exc_t, failed_table_op_exc_t,
            maybe_failed_table_op_exc_t, admin_op_exc_t);

    rdb_context_t *rdb_context;
    real_reql_cluster_interface_t *reql_cluster_interface;
    server_config_client_t *server_config_client;
};

#endif /* CLUSTERING_ADMINISTRATION_TABLES_TABLE_CONFIG_HPP_ */

