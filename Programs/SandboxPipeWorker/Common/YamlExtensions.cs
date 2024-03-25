using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using YamlDotNet.RepresentationModel;

namespace SandboxPipeWorker.Common
{
    public static class YamlExtensions
    {
        public static bool TryGetValue(this YamlMappingNode yamlMappingNode, string key, out YamlScalarNode scalar)
        {

            if (yamlMappingNode.Children.ContainsKey(key) && yamlMappingNode[key] is YamlScalarNode yamlScalarNode)
            {
                scalar = yamlScalarNode;
                return true;
            }
            scalar = null!;
            return false;
        }

        public static bool TryGetArray(this YamlMappingNode yamlMappingNode, string key, out YamlSequenceNode sequence)
        {
            if (yamlMappingNode.Children.ContainsKey(key) && yamlMappingNode[key] is YamlSequenceNode yamlSequenceNode)
            {
                sequence = yamlSequenceNode;
                return true;
            }
            sequence = null!;
            return false;
        }
    }
}
