using Mile.Project.Helpers;
using System.Xml;

namespace NanaGet.RefreshPackageVersion
{
    internal class Program
    {
        static (int Major, int Minor) Version = (1, 2);
        static string BuildStartDay = "2022-04-25";

        static string GenerateVersionString()
        {
            return string.Format(
                "{0}.{1}.{2}.0",
                Version.Major,
                Version.Minor,
                DateTime.Today.Subtract(DateTime.Parse(BuildStartDay)).TotalDays);
        }

        static string RepositoryRoot = GitRepository.GetRootPath();

        static void RefreshAppxManifestVersion()
        {
            string FilePath = string.Format(
                @"{0}\NanaGetPackage\Package.appxmanifest",
                RepositoryRoot);

            XmlDocument Document = new XmlDocument();
            Document.PreserveWhitespace = true;
            Document.Load(FilePath);

            XmlNode? PackageNode = Document["Package"];
            if (PackageNode != null)
            {
                XmlNode? IdentityNode = PackageNode["Identity"];
                if (IdentityNode != null)
                {
                    XmlAttribute? VersionAttribute =
                        IdentityNode.Attributes["Version"];
                    if (VersionAttribute != null)
                    {
                        FileUtilities.SaveTextToFileAsUtf8Bom(
                            FilePath,
                            File.ReadAllText(FilePath).Replace(
                                VersionAttribute.Value,
                                GenerateVersionString()));
                    }
                }
            }
        }

        static void Main(string[] args)
        {
            RefreshAppxManifestVersion();

            Console.WriteLine(
                "NanaGet.RefreshPackageVersion task has been completed.");
            Console.ReadKey();
        }
    }
}
