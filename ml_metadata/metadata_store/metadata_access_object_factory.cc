/* Copyright 2019 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "ml_metadata/metadata_store/metadata_access_object_factory.h"

#include <memory>

#include "absl/memory/memory.h"
#include "ml_metadata/metadata_store/query_config_executor.h"
#include "ml_metadata/metadata_store/rdbms_metadata_access_object.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"

namespace ml_metadata {

namespace {
// Creates MetadataAccessObject (MAO) with the input query config and the
// MetadataSource and returns the created MAO pointer.
// The query config has template queries and other config information.
// The MetadataSource is used to execute specific queries.
tensorflow::Status CreateRDBMSMetadataAccessObject(
    const MetadataSourceQueryConfig& query_config,
    MetadataSource* const metadata_source,
    std::unique_ptr<MetadataAccessObject>* result) {
  if (!metadata_source->is_connected())
    TF_RETURN_IF_ERROR(metadata_source->Connect());
  std::unique_ptr<QueryExecutor> executor = absl::WrapUnique(
        new QueryConfigExecutor(query_config, metadata_source));
  *result =
      absl::WrapUnique(new RDBMSMetadataAccessObject(std::move(executor)));
  return tensorflow::Status::OK();
}


}  // namespace

tensorflow::Status CreateMetadataAccessObject(
    const MetadataSourceQueryConfig& query_config,
    MetadataSource* const metadata_source,
    std::unique_ptr<MetadataAccessObject>* result) {
  switch (query_config.metadata_source_type()) {
    case UNKNOWN_METADATA_SOURCE:
      return tensorflow::errors::InvalidArgument(
          "Metadata source type is not specified.");
    case MYSQL_METADATA_SOURCE:
      return CreateRDBMSMetadataAccessObject(query_config, metadata_source,
                                             result);
    case SQLITE_METADATA_SOURCE:
      return CreateRDBMSMetadataAccessObject(query_config, metadata_source,
                                             result);
    default:
      return tensorflow::errors::Unimplemented("Unknown Metadata source type.");
  }
}

}  // namespace ml_metadata
